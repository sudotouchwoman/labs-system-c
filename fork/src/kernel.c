#include "kernel.h"

#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

int is_readable_fd(const int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags == -1) return 0;
    return (flags & O_ACCMODE) == O_RDONLY || (flags & O_ACCMODE) == O_RDWR;
}

services_t * make_pool() {
    services_t * pool = calloc(1, sizeof(services_t));
    if (pool == NULL)
        return NULL;
    pool->pids = NULL;
    pool->n_services = 0;
    return pool;
}

int init_services(services_t *const pool, const size_t service_count) {
    pid_t * pids = calloc(service_count, sizeof(pid_t));
    if (pids == NULL)
        return KERNEL_SERVICE_INIT_ERROR;
    free(pool->pids);
    pool->pids = pids;
    pool->n_services = service_count;
    return KERNEL_OK;
}

void release_services(services_t *const pool) {
    if (pool == NULL) return;
    free(pool->pids);
    free(pool);
}

int spawn_child_processes(services_t *const pool) {
    pid_t pid = 0;
    for (size_t i = 0; i < N_PROCESSES; ++i) {
        if ((pid = fork()) == -1) return KERNEL_FORK_ERROR;
        if (pid == 0) break;
        pool->pids[i] = pid;
    }
    return KERNEL_OK;
}

int call_service(const services_t pool, const size_t id) {
    kill(pool.pids[id], SIGUSR1);
    return 0;
}

void kill_services(const services_t pool) {
    for (size_t i = 0; i < pool.n_services; ++i) {
        kill(pool.pids[i], SIGTERM);
    }
}

