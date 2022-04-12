#ifndef KERNEL_H
#define KERNEL_H

#include <stdlib.h>

#define N_PROCESSES 4

enum {
    KERNEL_FORK_ERROR = -1,
    KERNEL_SERVICE_INIT_ERROR = -1,
    KERNEL_OK = 0
};

typedef struct {
    size_t  n_services;
    pid_t* pids;
} services_t;

services_t * make_pool();
int init_services(services_t *const pool, const size_t service_count);
void release_services(services_t * const pool);

int spawn_child_processes(services_t *const pool);
int call_service(const services_t pool, const size_t service_id);
void kill_services(const services_t pool);

int is_readable_fd(const int fd);

#endif