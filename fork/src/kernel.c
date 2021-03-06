#include "kernel.h"

#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>

static const char TIME_FORMAT[] = "[%02d:%02d:%02d] ";
static const size_t TIME_BUFFER_SIZE = 12;

// empty signal handler to wake up from pause()
// after child service finishes its work
static void kernel_sighandler(int signum) {
    return;
}

void listen_to_children() {
    signal(SIGUSR1, kernel_sighandler);
}

static void format_time(char * const buffer) {
    // this function is actually static thus
    // not intended for outer use, however, it is
    // a good trait to ensure that the buffer is non-empty
    if (buffer == NULL) return;

    time_t rawtime;
    struct tm time_info;

    // use localtime_r in order to avoid
    // odd memory allocation. localtime functions work in a wierd manner
    // actually, see https://stackoverflow.com/questions/23541583/localtime-r-consuming-some-memory-before-program-exit
    // for more details
    time(&rawtime);
    localtime_r(&rawtime, &time_info);
    // write no more than the expected length
    // of time-format
    snprintf(
        buffer,
        TIME_BUFFER_SIZE,
        TIME_FORMAT,
        time_info.tm_hour,
        time_info.tm_min,
        time_info.tm_sec
    );
}

int write_time(const int fd) {
    if (!is_writeable_fd(fd)) return KERNRL_WRONG_FD_MODE_ERROR;

    char time_buffer[TIME_BUFFER_SIZE];
    format_time(time_buffer);
    write(fd, time_buffer, sizeof(time_buffer));

    return KERNEL_OK;
}

proc_pool_t * make_pool() {
    proc_pool_t * pool = calloc(1, sizeof(proc_pool_t));
    if (pool == NULL)
        return NULL;
    pool->pids = NULL;
    pool->n_services = 0;
    return pool;
}

int init_services(proc_pool_t *const pool, const size_t service_count) {
    if (pool == NULL)
        return KERNEL_SERVICE_INIT_ERROR;
    pid_t * pids = calloc(service_count, sizeof(pid_t));
    if (pids == NULL)
        return KERNEL_SERVICE_INIT_ERROR;
    free(pool->pids);
    pool->pids = pids;
    pool->n_services = service_count;
    return KERNEL_OK;
}

void release_services(proc_pool_t *const pool) {
    if (pool == NULL) return;
    free(pool->pids);
    free(pool);
}

int spawn_child_processes(proc_pool_t *const pool, size_t * const this_pid) {
    if (pool == NULL) return KERNEL_FORK_ERROR;
    if (this_pid == NULL) return KERNEL_FORK_ERROR;

    pid_t pid = 0;
    for (size_t i = 0; i < pool->n_services; ++i) {
        if ((pid = fork()) == -1) return KERNEL_FORK_ERROR;
        if (pid == 0) {
            signal(SIGUSR1, work_handler);
            signal(SIGTERM, quit_handler);
            *this_pid = i;
            break;
        }
        pool->pids[i] = pid;
    }
    return pid;
}

int call_service(const proc_pool_t pool, const size_t id) {
    kill(pool.pids[id], SIGUSR1);
    return 0;
}

void kill_services(const proc_pool_t pool) {
    for (size_t i = 0; i < pool.n_services; ++i) {
        kill(pool.pids[i], SIGTERM);
    }
}

// ensure the file descriptor provided
// is readable/writeable (actually not used but useful)
int is_readable_fd(const int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) return KERNEL_FCNTL_FLAGS_ERROR;
    return (flags & O_ACCMODE) == O_RDONLY || (flags & O_ACCMODE) == O_RDWR;
}

int is_writeable_fd(const int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) return KERNEL_FCNTL_FLAGS_ERROR;
    return (flags & O_ACCMODE) == O_WRONLY || (flags & O_ACCMODE) == O_RDWR;
}

// fetch current tty attributes and store in
// provided termios structure
int get_tty(int fd, struct termios *const tty_p) {
    if (tty_p == NULL) return KERNEL_EMPTY_PTR_ERROR;
    if (!isatty(fd)) return KERNEL_FD_NOT_TERMINAL_ERROR;

    tcgetattr(fd, tty_p);
    return KERNEL_OK;
}

int apply_tty(int fd, const struct termios *const tty_p) {
    if (tty_p == NULL) return KERNEL_EMPTY_PTR_ERROR;
    if (!isatty(fd)) return KERNEL_FD_NOT_TERMINAL_ERROR;
    tcsetattr(fd, TCSAFLUSH, tty_p);
    return KERNEL_OK;
}

// adjust the termios structure attributes
// in order to enable the noncanonical input mode
int set_noncanonical_mode(struct termios * tty_p) {
    if (tty_p == NULL)
        return KERNEL_EMPTY_PTR_ERROR;
    // set noncanonical mode, remove echoing chars and
    // intercept signals generated by the terminal (e.g., 
    // SIGINT)
    tty_p->c_lflag &= ~(ICANON | ECHO | ISIG);
    tty_p->c_cc[VMIN] = 1;
    return KERNEL_OK;    
}

static int is_digit(char c) {
    return c >= ASCII_0 && c <= ASCII_9;
}

int ensure_user_input(char c) {
    if (!is_digit(c)) return KERNEL_BAD_INPUT;
    return (int)(c - ASCII_0) < N_SERVICES ? KERNEL_VALID_INPUT: KERNEL_BAD_INPUT;
}

