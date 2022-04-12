#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>

typedef struct {
    char * str;
    size_t length;
    size_t capacity;
} string_t;

string_t * empty_string(const size_t length);
string_t * make_string(char * raw_string);
string_t * read_string(const int fd);
string_t * shrink_string(const string_t * str);
void delete_string(string_t * str);

#endif
