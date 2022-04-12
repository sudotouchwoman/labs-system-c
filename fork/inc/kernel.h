#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>

#define N_PROCESSES 4

static pid_t child_processes[N_PROCESSES] = {0, 0, 0, 0};

int read_user_input();
int generate_child_processes();
int send_signal_to_child(const size_t child);
