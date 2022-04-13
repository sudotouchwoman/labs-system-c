#ifndef SERVICES_H
#define SERVICES_H

#include <signal.h>

#include "utils.h"

enum {
    SERVICES_STDIN = 0,
    SERVICES_STDOUT = 1,
    SERVICES_EMPTY_PTR_ERROR = -1,
    SERVICES_EMPTY_STRING = -2,
    SERVICES_OK = 0,
    ASCII_a = 97,
    ASCII_z = 97 + 25,
    ASCII_A = 65,
    ASCII_Z = 65 + 25
};

// signal handlers for services (child processes)
void quit_handler(int signum);
void work_handler(int signum);

// logic implemented by services
int swap_neighbours(string_t * const str);
int invert_string(string_t * const str);
int swap_register(string_t * const str);
int swap_charset(string_t * const str);

// execute logic based on service id
void use_service(const size_t id);

#endif
