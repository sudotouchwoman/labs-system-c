#include "kernel.h"

void register_swap_handler(int signum);
void string_inversion_handler(int signum);
void char_swap_handler(int signum);
void charset_swap_handler(int signum);

int swap_chars(char *const string, const size_t length);
int invert_string(char *const string, const size_t length);
int swap_register(char *const string, const size_t length);
int swap_charset(char *const string, const size_t length);
