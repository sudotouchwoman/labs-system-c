#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

// boundary conditions
// left edge
#define LEFT_1TYPE -30.0 // 1 kind BC
// #define LEFT_2TYPE 0   // 2 kind BC
// #define LEFT_3TYPE // 3 kind BC
// right edge
#define RIGHT_1TYPE 30.0
// #define RIGHT_2TYPE -5
// #define RIGHT_3TYPE

#define L (8.0 * 8.0)
// #define L (4096*4096)
#define at 2.0
#define dx 1.0
#define dt 0.25
#define T_MAX 100.0
// #define T_MAX 1024
#define T_0 0.0 // initial temperature value

const int xx = L / dx;
const int tt = T_MAX / dt;

#define MAKE_GNUPLOT 1 // whether to write to gnuplot file (0, 1)

double left_boundary(double next_node);
double right_boundary(double prev_node);

double forward_euler(double t_left, double t_mid, double t_right);

void swap_values(int rank, int total, double *T, double *stripe, int stripe_size, double *left, double *right);
void time_step(int rank, int total, double **stripe_old, double **stripe_new, int stripe_size, double left, double right);

void gnuplot_preamble();
void dump_timestep(FILE *f, double *T);

int main(int argc, char *argv[])
{
  int total = 0, rank = 0;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &total);

  if (xx % total != 0)
  {
    printf("The number of nodes must be a multiple of the number of processes!\n");
    MPI_Finalize();
  }

  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  double *T;
  if (!rank)
  {
    // check for numerical stability
    if (dt > (dx * dx / 2 / at))
    {
      fprintf(
          stderr,
          "With these values of [dt, dx, at] numeric solution diverges!\n"
          "Must be compliant to: dt < dx^2 / 2k\n"
          "Got this: dt=%.3lf, dx=%.3lf, at=%.3lf\n",
          dt, dx, at);
    }

    // allocate & init memory for buffer in root process
    // to store the result
    T = (double *)malloc(sizeof(double) * xx);
    for (int i = 0; i < xx; ++i)
    {
      T[i] = T_0;
    }
    T[0] = left_boundary(T[1]);
    T[xx - 1] = right_boundary(T[xx - 2]);
  }

  // allocate buffers in each worker (including root)
  // to store 2 timesteps for the working area of the process (size is N/M,
  // where N is number of nodes, M is number of workers)
  const int stripe_size = xx / total;
  double *stripe_old = (double *)malloc(sizeof(double) * stripe_size);
  double *stripe_new = (double *)malloc(sizeof(double) * stripe_size);
  // distribute the initial values of T among workers
  MPI_Scatter(T, stripe_size, MPI_DOUBLE, stripe_old, stripe_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  
  // vars to store values from neighbouring workers
  double left_from_stripe = 0, right_from_stripe = 0;
#if MAKE_GNUPLOT
  FILE *f = NULL;
  if (!rank)
  {
    gnuplot_preamble();
    f = fopen("temperature.dat", "w");
    if (f == NULL)
    {
      printf("Can't open/create temperature.dat file!\n");
    }
  }
#endif
  struct timeval begin, end;
  if (!rank)
    gettimeofday(&begin, NULL);

  // time integration
  double *tmp;
  for (int i = 0; i < tt; ++i)
  {
    swap_values(rank, total, T, stripe_old, stripe_size, &left_from_stripe, &right_from_stripe);
    time_step(rank, total, &stripe_old, &stripe_new, stripe_size, left_from_stripe, right_from_stripe);
#if MAKE_GNUPLOT
    MPI_Gather(stripe_old, stripe_size, MPI_DOUBLE, T, stripe_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    if (!rank)
    {
      dump_timestep(f, T);
    }
#endif
  }
  // gather the final values from workers and store in T
  MPI_Gather(stripe_old, stripe_size, MPI_DOUBLE, T, stripe_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  if (!rank)
  {
    gettimeofday(&end, NULL);
    const double elapsed = (end.tv_sec - begin.tv_sec) + (end.tv_usec - begin.tv_usec) / 1000000.0;
    fprintf(stdout, "Processes %3d | Time, ms: %.3f\n", total, elapsed);
  }
#if MAKE_GNUPLOT
  // release allocated resources
  if (!rank)
  {
    fclose(f);
    free(T);
  }
#endif
  free(stripe_old);
  free(stripe_new);
  MPI_Finalize();
}

void swap_values(
    int rank,
    int total,
    double *T,
    double *stripe,
    int stripe_size,
    double *left,
    double *right)
{
  // exchange values between neighbouring workers to
  // compute the derivative values at the edges of working domain
  MPI_Status stat;
  if (rank < total - 1)
  {
    MPI_Sendrecv(
      &stripe[stripe_size - 1], 1, MPI_DOUBLE, rank + 1, 0,
      right, 1, MPI_DOUBLE, rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
  }
  if (rank > 0)
  {
    MPI_Sendrecv(
      &stripe[0], 1, MPI_DOUBLE, rank - 1, 0,
      left, 1, MPI_DOUBLE, rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
  }
}
void time_step(
    int rank,
    int total,
    double **old_stripe,
    double **new_stripe,
    int stripe_size,
    double left,
    double right)
{
  // peroform single time integration step, update values in stripe_new
  // using values from stripe_old
  // pass double pointers to swap pointer values in the end
  // to avoid using memcpy
  double *const stripe_old = *old_stripe, *const stripe_new = *new_stripe;
  if (total > 1)
  {
    if (rank > 0)
    {
      stripe_new[0] = forward_euler(left, stripe_old[0], stripe_old[1]);
      if (rank == total - 1)
      {
        // update values using boundaries at 0 and last nodes
        stripe_new[stripe_size - 1] = right_boundary(stripe_old[stripe_size - 2]);
      }
    }
    if (rank < total - 1)
    {
      stripe_new[stripe_size - 1] = forward_euler(stripe_old[stripe_size - 2], stripe_old[stripe_size - 1], right);
      if (rank == 0)
      {
        stripe_new[0] = left_boundary(stripe_old[1]);
      }
    }
  }
  else
  {
    stripe_new[0] = left_boundary(stripe_old[1]);
    stripe_new[stripe_size - 1] = right_boundary(stripe_old[stripe_size - 2]);
  }
  // for the rest of the nodes, use neighbouring values to compute
  // 2nd derivative with respect to space and use it in forward euler formula
  for (int i = 1; i < stripe_size - 1; ++i)
  {
    stripe_new[i] = forward_euler(stripe_old[i - 1], stripe_old[i], stripe_old[i + 1]);
  }
  // swap pointer values as on the next step,
  // current values will become old values
  double *const tmp = *old_stripe;
  *old_stripe = *new_stripe;
  *new_stripe = tmp;
}

double forward_euler(double t_left, double t_mid, double t_right)
{
  // T^{i+1}_j = a_t * \frac{d^2T^i_j}{dx^2} + T^i_j
  // simple forward euler integration with central difference for 2nd derivative
  // approximation
  return at * (t_right - 2 * t_mid + t_left) * dt / dx / dx + t_mid;
}

double left_boundary(double next_node)
{
  double result = 0;
#ifdef LEFT_1TYPE
  result = LEFT_1TYPE;
#endif
  // 2 kind boundaries
#ifdef LEFT_2TYPE
  result = ((double)LEFT_2TYPE * dx) / at + next_node;
#endif
#ifdef LEFT_3TYPE
  // 3rd kind boundary on left side: at * (dT/dx) = F(T),
  // there F(T) = C1*T + C2. C1, C2 - some arbitrary constants
  const double C1 = 0.5;
  const double C2 = 0;
  result = (C2 * dx + next_node * at) / (at - C1 * dx);
#endif
  return result;
}

double right_boundary(double prev_node)
{
  double result;
#ifdef RIGHT_1TYPE
  result = RIGHT_1TYPE;
#endif
#ifdef RIGHT_2TYPE
  result = ((double)RIGHT_2TYPE * dx) / at + prev_node;
#endif
#ifdef RIGHT_3TYPE
  // 3rd kind boundary: at * (dT/dx) = F(T),
  // there F(T) = C1*T + C2. C1, C2 - some arbitrary constants
  const double C1 = 0.5;
  const double C2 = 0;
  result = (C2 * dx + prev_node * at) / (at - C1 * dx);
#endif
  return result;
}

void gnuplot_preamble()
{
  FILE *f = fopen("gnu.config", "w");
  if (f == NULL)
  {
    fprintf(stderr, "Cannot open/create gnu.config file!\n");
    return;
  }
  fprintf(f, "set terminal gif animate delay 2\n set style line 1 lc rgb '#5C4B51' pt 6 ps 1 lt 1 lw 2\n");
  fprintf(f, "set xrange [0:%d]\n", xx);
  fprintf(f, "set output 'out.gif'\n");
  fprintf(f, "stats 'temperature.dat' nooutput\n");
  fprintf(f, "do for [i=0:%d] {\n", tt - 1);
  fprintf(f, "plot 'temperature.dat' index i w l ls 1 title 'T distribution'\n}");
  fclose(f);
}

void dump_timestep(FILE *f, double *T)
{
  for (int i = 0; i < xx; ++i)
  {
    fprintf(f, "%d %lf\n", i, T[i]);
  }
  fprintf(f, "\n\n");
}
