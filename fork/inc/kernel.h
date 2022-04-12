#ifndef KERNEL_H
#define KERNEL_H

#include "utils.h"

#define N_PROCESSES 4

pid_t child_processes[N_PROCESSES] = {0, 0, 0, 0};

int generate_child_processes();
int send_signal_to_child(const size_t child);

#endif