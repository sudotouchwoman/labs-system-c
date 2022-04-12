#include <stdio.h>
#include <stdlib.h>

#include "kernel.h"

int main(int argc, char *argv[]) {
    fprintf(stderr, "enter string: ");
    string_t * str = read_string(0);
    fprintf(stderr, " you entered: %s", str->str);
    delete_string(str);
    return EXIT_SUCCESS;
}
