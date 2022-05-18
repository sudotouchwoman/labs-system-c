#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
// boundary conditions

// left edge
#define LEFT_1TYPE 100 // ГУ первого рода
// #define LEFT_2TYPE 5 // ГУ второго рода
// #define LEFT_3TYPE  // ГУ третьего рода
// right edge
#define RIGHT_1TYPE 30
// #define RIGHT_2TYPE 25
// #define RIGHT_3TYPE

#define L 200
#define at 2
#define dx 1
#define dt 0.1
#define T_MAX 20
#define T_0 20 // initial temperature value

const int xx = L / dx;
const int tt = T_MAX / dt;

#define MAKE_GNUPLOT 1 // Писать или нет в gnuplot файл (0, 1)

void apply_boundary_conds(double *T);

double central_difference(double t_left, double t_mid, double t_right);
double left_border(double next_node);
double right_border(double prev_node);

void values_exchange(int rank, int total, double *T, double *stripe,
                     int stripe_size, double *left, double *right);
void calculate_temperature(int rank, int total, double *stripe_old,
                           double *stripe_new, double *tmp, int stripe_size,
                           double left, double right);

void create_gnuplot_config();
void write_data_to_file(FILE *f, double *T);

int main(int argc, char **argv)
{
  MPI_Init(&argc, &argv);
  int total = 0, rank = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &total);
  if (xx % total != 0)
  {
    printf("The number of nodes is not a multiple of the number of processes!\n");
    MPI_Finalize();
  }
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  double *T;
  if (!rank)
  {
    if (dt > (dx*dx / (2 * at)))
    {
      fprintf(stderr, "With these values of dt, dx, at the result is inaccurate!\n");
    }

    T = (double *)malloc(sizeof(double) * xx);
    for (int i = 0; i < xx; ++i)
    {
      T[i] = T_0;
    }
    T[0] = left_border(T[1]);
    T[xx - 1] = right_border(T[xx - 2]);
  }
  int stripe_size = xx / total;
  double *stripe_old = (double *)malloc(sizeof(double) * stripe_size);
  double *stripe_new = (double *)malloc(sizeof(double) * stripe_size);
  MPI_Scatter(T, stripe_size, MPI_DOUBLE, stripe_old, stripe_size, MPI_DOUBLE,
              0, MPI_COMM_WORLD);
  double left_from_stripe, right_from_stripe;
#if MAKE_GNUPLOT
  FILE *f;
  if (!rank)
  {
    create_gnuplot_config();
    f = fopen("plotting_data.dat", "w");
    if (f == NULL)
    {
      printf("Can't open/create plotting_data.dat file!\n");
    }
  }
#endif
  struct timeval begin, end;
  if (!rank)
    gettimeofday(&begin, NULL);

  double *tmp;
  for (int i = 0; i < tt; ++i)
  {
    values_exchange(rank, total, T, stripe_old, stripe_size, &left_from_stripe,
                    &right_from_stripe);
    calculate_temperature(rank, total, stripe_old, stripe_new, tmp, stripe_size,
                          left_from_stripe, right_from_stripe);
#if MAKE_GNUPLOT
    MPI_Gather(stripe_new, stripe_size, MPI_DOUBLE, T, stripe_size, MPI_DOUBLE,
               0, MPI_COMM_WORLD);
    if (!rank)
    {
      write_data_to_file(f, T);
    }
#endif
  }
  MPI_Gather(stripe_new, stripe_size, MPI_DOUBLE, T, stripe_size, MPI_DOUBLE, 0,
             MPI_COMM_WORLD);
  double elapsed;
  if (!rank)
  {
    gettimeofday(&end, NULL);
    elapsed = (end.tv_sec - begin.tv_sec) +
              ((end.tv_usec - begin.tv_usec) / 1000000.0);
    printf("Processes %3d | Time, ms: %.3f\n", total, elapsed);
  }
#if MAKE_GNUPLOT
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

void values_exchange(
  int rank,
  int total,
  double *T,
  double *stripe,
  int stripe_size,
  double *left,
  double *right
)
{
  MPI_Status stat;
  if (rank < total - 1)
  {
    MPI_Sendrecv(&stripe[stripe_size - 1], 1, MPI_DOUBLE, rank + 1, 0, right, 1,
                 MPI_DOUBLE, rank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
  }
  if (rank > 0)
  {
    MPI_Sendrecv(&stripe[0], 1, MPI_DOUBLE, rank - 1, 0, left, 1, MPI_DOUBLE,
                 rank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
  }
}
void calculate_temperature(
  int rank,
  int total,
  double *stripe_old,
  double *stripe_new,
  double *tmp,
  int stripe_size,
  double left,
  double right
)
{
  if (total > 1)
  {
    if (rank > 0)
    {
      stripe_new[0] = central_difference(left, stripe_old[0], stripe_old[1]);
      if (rank == total - 1)
      {
        stripe_new[stripe_size - 1] = right_border(stripe_old[stripe_size - 2]);
      }
    }
    if (rank < total - 1)
    {
      stripe_new[stripe_size - 1] = central_difference(
          stripe_old[stripe_size - 2], stripe_old[stripe_size - 1], right);
      if (rank == 0)
      {
        stripe_new[0] = left_border(stripe_old[1]);
      }
    }
  }
  else
  {
    stripe_new[0] = left_border(stripe_old[1]);
    stripe_new[stripe_size - 1] = right_border(stripe_old[stripe_size - 2]);
  }
  for (int i = 1; i < stripe_size - 1; ++i)
  {
    stripe_new[i] =
        central_difference(stripe_old[i - 1], stripe_old[i], stripe_old[i + 1]);
  }
  memcpy(stripe_old, stripe_new, sizeof(double) * stripe_size);
  tmp = stripe_old;
  stripe_old = stripe_new;
  stripe_new = tmp;
}

double central_difference(double t_left, double t_mid, double t_right)
{
  return at * (t_right - 2 * t_mid + t_left) * dt / dx / dx + t_mid;
}
double left_border(double next_node)
{
  double result = 0;
#ifdef LEFT_1TYPE
  result = LEFT_1TYPE;
#endif
  // 2 type boundaries
#ifdef LEFT_2TYPE
  result = ((double)LEFT_2TYPE * dx) / at + next_node;
#endif
#ifdef LEFT_3TYPE
  // 3rd type boundary on left side: at * (dT/dx) = F(T),
  // there F(T) = C1*T + C2. C1, C2 - some arbitrary constants
  const double C1 = 0.5;
  const double C2 = 5;
  result = (C2 * dx + next_node * at) / (at - C1 * dx);
#endif
  return result;
}

double right_border(double prev_node)
{
  double result;
#ifdef RIGHT_1TYPE
  result = RIGHT_1TYPE;
#endif
#ifdef RIGHT_2TYPE
  result = ((double)RIGHT_2TYPE * dx) / at + prev_node;
#endif
#ifdef RIGHT_3TYPE
  // 3rd type boundary: at * (dT/dx) = F(T),
  // there F(T) = C1*T + C2. C1, C2 - some arbitrary constants
  const double C1 = 0.5;
  const double C2 = 5;
  result = (C2 * dx + prev_node * at) / (at - C1 * dx);
#endif
  return result;
}

void create_gnuplot_config()
{
  FILE *f = fopen("gnu.config", "w");
  if (f == NULL)
  {
    fprintf(stderr, "Cannot open/create gnu.config file!\n");
    return;
  }
  fprintf(f, "set terminal gif animate delay 10\n");
  fprintf(f, "set output 'out.gif'\n");
  fprintf(f, "stats ");
  fprintf(f, "'plotting_data.dat'");
  fprintf(f, " nooutput\n");
  fprintf(f, "do for [i=0:%d] {\n", tt - 1);
  fprintf(f, "plot 'plotting_data.dat' index i with lines\n}");
  fclose(f);
}

void write_data_to_file(FILE *f, double *T)
{
  for (int i = 0; i < xx; ++i)
  {
    fprintf(f, "%d %f\n", i, T[i]);
  }
  fprintf(f, "\n\n");
}
