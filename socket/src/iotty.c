#include "iotty.h"

static const char PROMPT[] =
    "Welcome to HTTP client\n"
    ;

void prompt(const int fd) {
    write(fd, PROMPT, sizeof(PROMPT));
}

static const char USAGE[] =
    "Usage: URL [port]\n";

void usage(const int fd) {
    write(fd, USAGE, sizeof(USAGE));
}
