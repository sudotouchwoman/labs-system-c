#ifndef KERNEL_H
#define KERNEL_H

#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

#include "services.h"

#define N_SERVICES 4

enum {
    KERNEL_FORK_ERROR = -1,
    KERNEL_SERVICE_INIT_ERROR = -1,
    KERNEL_OK = 0,
    KERNEL_EMPTY_PTR_ERROR,
    KERNEL_FD_NOT_TERMINAL_ERROR = -1,
    KERNEL_FCNTL_FLAGS_ERROR = 0,
    CHILD = 0
};

enum {
    ASCII_0 = 48,
    ASCII_9 = 48 + 9,
    KERNEL_BAD_INPUT = -1
};

typedef struct {
    size_t  n_services;
    pid_t* pids;
} proc_pool_t;

proc_pool_t * make_pool();
int init_services(proc_pool_t *const pool, const size_t service_count);
void release_services(proc_pool_t * const pool);

int spawn_child_processes(proc_pool_t *const pool);
int call_service(const proc_pool_t pool, const size_t service_id);
void kill_services(const proc_pool_t pool);

int is_readable_fd(const int fd);

int get_tty(int fd, struct termios * tty_p);
int set_noncanonical_mode(struct termios * tty_p);
int apply_tty(int fd, struct termios * tty_p);

int ensure_user_input(char c);

#endif