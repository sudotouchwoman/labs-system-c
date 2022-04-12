#ifndef SERVICES_H
#define SERVICES_H

#include <signal.h>

#include "utils.h"

void register_swap_handler(int signum);
void string_inversion_handler(int signum);
void char_swap_handler(int signum);
void charset_swap_handler(int signum);

int swap_chars(string_t * const str);
int invert_string(string_t * const str);
int swap_register(string_t * const str);
int swap_charset(string_t * const str);

// static int (*actions[])(string_t * const) = {
//     swap_chars, invert_string, swap_register, swap_charset
// };

#endif
