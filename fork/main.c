#include <stdio.h>

#include "kernel.h"
#include "services.h"

int main(int argc, char* argv[]) {
    struct termios old_tty, tty;
    if (get_tty(SERVICES_STDIN, &tty) != KERNEL_OK) {
        fprintf(stderr, "Failed to obtain tty properties. Are we running from terminal?\n");
        return EXIT_FAILURE;
    }

    proc_pool_t * pool = make_pool();
    if (pool == NULL) {
        fprintf(stderr, "Unable to create process pool\n");
        return EXIT_FAILURE;
    }

    pid_t pid = 0;

    init_services(pool, N_SERVICES);
    if ((pid = spawn_child_processes(pool)) == KERNEL_FORK_ERROR) {
        kill_services(*pool);
        release_services(pool);
        fprintf(stderr, "Fork error\n");
        return EXIT_FAILURE;
    }

    if (pid == CHILD) {
        fprintf(stderr, "Spawned Service ==%d==\n", getpid());
        for (;;) {
            pause();
        }
    }

    old_tty = tty;
    set_noncanonical_mode(&tty);

    for (;;) {
        apply_tty(SERVICES_STDIN, &tty);
        fprintf(stderr, "Awaiting User input (integer in rage [0; %d])\n", N_SERVICES - 1);
        char c = 0;
        read(SERVICES_STDIN, &c, sizeof(c));
        if (c == 'Q' || c == 'q') break;
        if (ensure_user_input(c) == KERNEL_BAD_INPUT) {
            fprintf(stderr, "UNKNOWN SYMBOL PROVIDED:\t %c\n", c);
            continue;
        }
        apply_tty(SERVICES_STDIN, &old_tty);
        call_service(*pool, (size_t)atoi(&c));
    }

    kill_services(*pool);    
    release_services(pool);

    apply_tty(SERVICES_STDIN, &old_tty);
}
