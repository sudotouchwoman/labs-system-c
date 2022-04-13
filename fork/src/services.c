#include "services.h"

#include <stdio.h>
#include <unistd.h>

// this source file contains logic implemented
// by the child processes (referred to as services)

void quit_handler(int signum) {
    fprintf(stderr, "%d ==> Service Exiting...\n", getpid());
    exit(EXIT_SUCCESS);
}

void work_handler(int signum) {
    fprintf(stderr, "%d ==> Caught signal from kernel\n", getpid());
}

static void swap(char *const a, char *const b) {
    if (a == b) return;
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

static void register_swap_handler() {
    fprintf(stderr, "%d ==> REGISTER SWAPPER\n", getpid());
    string_t * str = read_string(SERVICES_STDIN);
    if (swap_register(str) != SERVICES_OK) {
        fprintf(stderr, "%d ==> Could not process the input!\n", getpid());
        delete_string(str);
        return;
    }
    fprintf(stderr, "%d ==> Processed string: %s\n", getpid(), str->raw_string);
    delete_string(str);
}

static void string_inversion_handler() {
    fprintf(stderr, "%d ==> STRING INVERTOR\n", getpid());
    string_t * str = read_string(SERVICES_STDIN);
    if (invert_string(str) != SERVICES_OK) {
        fprintf(stderr, "%d ==> Could not process the input!\n", getpid());
        delete_string(str);
        return;
    }
    fprintf(stderr, "Processed string: %s\n", str->raw_string);
    delete_string(str);
}

static void char_swap_handler() {
    fprintf(stderr, "%d ==> EVEN-ODD SWAPPER\n", getpid());
    string_t * str = read_string(SERVICES_STDIN);
    if (swap_neighbours(str) != SERVICES_OK) {
        fprintf(stderr, "%d ==> Could not process the input!\n", getpid());
        delete_string(str);
        return;
    }
    fprintf(stderr, "%d ==> Processed string: %s\n", getpid(), str->raw_string);
    delete_string(str);
}

static void charset_swap_handler() {
    fprintf(stderr, "%d ==> CHARSET SWAPPER\n", getpid());
    string_t * str = read_string(SERVICES_STDIN);
    if (swap_charset(str) != SERVICES_OK) {
        fprintf(stderr, "%d ==> Could not process the input!\n", getpid());
        delete_string(str);
        return;
    }
    fprintf(stderr, "%d ==> Processed string: %s\n", getpid(), str->raw_string);
    delete_string(str);
}

int swap_neighbours(string_t * const str) {
    if (str == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->raw_string == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->length == 0) return SERVICES_EMPTY_STRING;

    for (size_t i = 0; i < str->length - 1; i += 2) {
        swap(str->raw_string + i, str->raw_string + i + 1);
    }
    return SERVICES_OK;
}

int invert_string(string_t * const str) {
    if (str == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->raw_string == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->length == 0) return SERVICES_EMPTY_STRING;

    for (size_t i = 0; i < str->length / 2; ++i) {
        swap(str->raw_string + i, str->raw_string + str->length - i - 1);
    }
    return SERVICES_OK;
}

static int is_lower_alpha(char c) {
    return c >= ASCII_a && c <= ASCII_z;
}

static int is_upper_alpha(char c) {
    return c >= ASCII_A && c <= ASCII_Z;
}

int swap_register(string_t * const str) {
    if (str == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->raw_string == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->length == 0) return SERVICES_EMPTY_STRING;

    for (size_t i = 0; i < str->length; ++i) {
        char c = str->raw_string[i];
        if (is_upper_alpha(c)) {
            str->raw_string[i] += 32;
            continue;
        }
        if (is_lower_alpha(c)) {
            str->raw_string[i] -= 32;
            continue;
        }   
    }
    return SERVICES_OK;
}

int swap_charset(string_t * const str) {
    if (str == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->raw_string == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->length == 0) return SERVICES_EMPTY_STRING;

    for (size_t i = 0; i < str->length; ++i) {
        str->raw_string[i] |= (1 << 7);
    }
    return SERVICES_OK;
}

// this is more a hacky way,
// but for a fixed number of services and corresponding actions,
// one can access the action of a service simply via index
static void (*actions[])() = {
    register_swap_handler, string_inversion_handler, char_swap_handler, charset_swap_handler
};

void use_service(const size_t id) {
    actions[id]();
}
