#include "kernel.h"

int generate_child_processes() {
    for (size_t i = 0; i < N_PROCESSES; ++i) {
    }
    return 0;
}

int send_signal_to_child(const size_t child) {
    kill(child_processes[child], SIGUSR1);
    return 0;
}
