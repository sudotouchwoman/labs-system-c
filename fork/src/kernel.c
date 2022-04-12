#include "kernel.h"

#include <fcntl.h>

proc_pool_t * make_pool() {
    proc_pool_t * pool = calloc(1, sizeof(proc_pool_t));
    if (pool == NULL)
        return NULL;
    pool->pids = NULL;
    pool->n_services = 0;
    return pool;
}

int init_services(proc_pool_t *const pool, const size_t service_count) {
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

int spawn_child_processes(proc_pool_t *const pool) {
    pid_t pid = 0;
    for (size_t i = 0; i < pool->n_services; ++i) {
        if ((pid = fork()) == -1) return KERNEL_FORK_ERROR;
        if (pid == 0) {
            set_handler(i);
            signal(SIGTERM, quit_handler);
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

int is_readable_fd(const int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) return KERNEL_FCNTL_FLAGS_ERROR;
    return (flags & O_ACCMODE) == O_RDONLY || (flags & O_ACCMODE) == O_RDWR;
}

int get_tty(int fd, struct termios * tty_p) {
    if (tty_p == NULL) return KERNEL_EMPTY_PTR_ERROR;
    if (!isatty(fd)) return KERNEL_FD_NOT_TERMINAL_ERROR;

    tcgetattr(fd, tty_p);
    return KERNEL_OK;
}

int set_noncanonical_mode(struct termios * tty_p) {
    if (tty_p == NULL)
        return KERNEL_EMPTY_PTR_ERROR;
    tty_p->c_lflag &= ~(ICANON | ECHO | ISIG);
    tty_p->c_cc[VMIN] = 1;
    return KERNEL_OK;    
}

int apply_tty(int fd, struct termios * tty_p) {
    if (tty_p == NULL) return KERNEL_EMPTY_PTR_ERROR;
    if (!isatty(fd)) return KERNEL_FD_NOT_TERMINAL_ERROR;
    tcsetattr(fd, TCSAFLUSH, tty_p);
    return KERNEL_OK;
}

static int is_digit(char c) {
    return c > ASCII_0 || c < ASCII_9;
}

int ensure_user_input(char c) {
    if (!is_digit(c)) return KERNEL_BAD_INPUT;
    return atoi(&c) < N_SERVICES ? KERNEL_OK: KERNEL_BAD_INPUT;
}

