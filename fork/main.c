#include <stdio.h>

#include "kernel.h"
#include "services.h"

int main(int argc, char* argv[]) {
    // get terminal settings
    // and apply noncanonical input mode in kernel process
    struct termios old_tty, tty;
    // ensure that we are running from the terminal, not from file
    if (get_tty(STDOUT_FILENO, &tty) != KERNEL_OK) {
        fprintf(stderr, "Failed to obtain tty properties of standard output. Are we running from terminal?\n");
        return EXIT_FAILURE;
    }

    if (get_tty(STDIN_FILENO, &tty) != KERNEL_OK) {
        fprintf(stderr, "Failed to obtain tty properties of standard input. Are we running from terminal?\n");
        return EXIT_FAILURE;
    }

    old_tty = tty;
    set_noncanonical_mode(&tty);
    apply_tty(STDIN_FILENO, &tty);

    // initialize process pool with
    // required number of cells for pids
    proc_pool_t * pool = make_pool();
    if (init_services(pool, N_SERVICES) == KERNEL_SERVICE_INIT_ERROR) {
        release_services(pool);
        apply_tty(STDIN_FILENO, &old_tty);
        fprintf(stderr, "KERNEL: nable to create process pool\n");
        return EXIT_FAILURE;
    }

    // set signal handler for kernel process
    // to wake up when service finishes processing
    pid_t pid = 0, ppid = getpid();
    listen_to_children();

    const char welcome_text[] = 
        "KERNEL: Welcome to Useless OS\n"
        "Useless OS can perform operations on strings:\n"
        "\t'0' - swap register\n"
        "\t'1' - invert string\n"
        "\t'2' - swap odd characters with even\n"
        "\t'3' - swap charset (to KOI-8)\n"
        "\t'Q' or 'q' to exit\n";
    write_time(STDOUT_FILENO);
    write(STDOUT_FILENO, welcome_text, sizeof(welcome_text));

    size_t service_id = 0;
    // spawn given number of service processes;
    // correctly handle the forking error case
    if ((pid = spawn_child_processes(pool, &service_id)) == KERNEL_FORK_ERROR) {
        kill_services(*pool);
        release_services(pool);
        apply_tty(STDIN_FILENO, &old_tty);
        fprintf(stderr, "KERNEL: Fork error\n");
        return EXIT_FAILURE;
    }

    // infinite loop for service processes:
    // await signal from kernel, perform operation
    // based on the id index (in this example, id has range from 0 to 3)
    if (pid == CHILD) {
        // release unused resources allocated
        // by kernel process
        release_services(pool);
        fprintf(stderr, "Spawned Service ==%d==\n", getpid());
        for (;;) {
            pause();
            apply_tty(STDIN_FILENO, &old_tty);
            use_service(service_id);
            // throw a debug message and
            // wake up kernel as soon as the work is done
            fprintf(stderr, "%d ==> Finished using service, passing signal to kernel\n", getpid());
            kill(ppid, SIGUSR1);
        }
    }

    // infinite loop for kernel process:
    // await user input in noncanonical mode,
    // read and parse single character from stdio (fd 0)
    // stop when Q key is pressed
    for (;;) {
        apply_tty(STDIN_FILENO, &tty);
        write_time(STDOUT_FILENO);
        fprintf(stderr, "KERNEL: Awaiting User input (integer in range [0; %d])\n", N_SERVICES - 1);
        char c = 0;
        read(STDIN_FILENO, &c, sizeof(c));
        if (c == 'Q' || c == 'q') break;
        if (ensure_user_input(c) == KERNEL_BAD_INPUT) {
            write_time(STDOUT_FILENO);
            fprintf(stderr, "KERNEL: Unknown option provided: %c\n", c);
            continue;
        }
        // suspend in order to release the terminal
        // and let service read from stdio in canonical mode
        call_service(*pool, (size_t)atoi(&c));
        pause();
    }

    const char quit_text[] = "KERNEL: See you Space Cowboy...\n";
    write_time(STDOUT_FILENO);
    write(STDOUT_FILENO, quit_text, sizeof(quit_text));

    // release resources and free allocated memory
    // also reset the terminal to its initial mode
    kill_services(*pool);
    release_services(pool);
    apply_tty(STDIN_FILENO, &old_tty);
    return EXIT_SUCCESS;
}
