#include "services.h"

#include <stdio.h>
#include <unistd.h>

void quit_handler(int signum) {
    exit(EXIT_SUCCESS);
}

static void swap(char *const a, char *const b) {
    if (a == b) return;
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

void register_swap_handler(int signum) {
    fprintf(stderr, "%d ==> REGISTER SWAPPER\n", getpid());
    string_t * str = read_string(SERVICES_STDIN);
    if (swap_register(str) != SERVICES_OK) {
        fprintf(stderr, "Could not process the input!\n");
        return;
    }
    fprintf(stderr, "%d ==> Processed string:\t%s\n", getpid(), str->raw_string);
}

void string_inversion_handler(int signum) {
    fprintf(stderr, "%d ==> STRING INVERTOR\n", getpid());
    string_t * str = read_string(SERVICES_STDIN);
    if (invert_string(str) != SERVICES_OK) {
        fprintf(stderr, "%d ==> Could not process the input!\n", getpid());
        return;
    }
    fprintf(stderr, "Processed string:\t%s\n", str->raw_string);
}

void char_swap_handler(int signum) {
    fprintf(stderr, "%d ==> EVEN-ODD SWAPPER\n", getpid());
    string_t * str = read_string(SERVICES_STDIN);
    if (swap_neighbours(str) != SERVICES_OK) {
        fprintf(stderr, "%d ==> Could not process the input!\n", getpid());
        return;
    }
    fprintf(stderr, "%d ==> Processed string:\t%s\n", getpid(), str->raw_string);
}

void charset_swap_handler(int signum) {
    fprintf(stderr, "%d ==> CHARSET SWAPPER\n", getpid());
    string_t * str = read_string(SERVICES_STDIN);
    if (swap_charset(str) != SERVICES_OK) {
        fprintf(stderr, "%d ==> Could not process the input!\n", getpid());
        return;
    }
    fprintf(stderr, "%d ==> Processed string:\t%s\n", getpid(), str->raw_string);
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

    for (size_t i = 0; i < str->length; ++i) {
        swap(str->raw_string + i, str->raw_string + str->length - i);
    }
    return SERVICES_OK;
}

static int is_lower_alpha(char c) {
    return c > ASCII_a || c < ASCII_z;
}

static int is_upper_alpha(char c) {
    return c > ASCII_A || c < ASCII_Z;
}

int swap_register(string_t * const str) {
    if (str == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->raw_string == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->length == 0) return SERVICES_EMPTY_STRING;

    for (size_t i = 0; i < str->length; ++i) {
        char c = str->raw_string[i];
        if (is_lower_alpha(c)) str->raw_string[i] -= 32;
        if (is_upper_alpha(c)) str->raw_string[i] += 32;
    }
    return SERVICES_OK;
}

int swap_charset(string_t * const str) {
    if (str == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->raw_string == NULL) return SERVICES_EMPTY_PTR_ERROR;
    if (str->length == 0) return SERVICES_EMPTY_STRING;

    for (size_t i = 0; i < str->length; ++i) {
        str->raw_string[i] &= (1 << 7);
    }
    return SERVICES_OK;
}

static void (*actions[])(int) = {
    register_swap_handler, string_inversion_handler, char_swap_handler, charset_swap_handler
};

void set_handler(const size_t id) {
    signal(SIGUSR1, *actions[id]);
}
