#include "utils.h"

#include <string.h>
#include <unistd.h>

string_t * empty_string(const size_t length) {
    string_t * new_string = calloc(1, sizeof(string_t));
    if (new_string == NULL)
        return NULL;
    
    char * inner_str = calloc(length, sizeof(char));
    if (inner_str == NULL) {
        free(new_string);
        return NULL;
    }

    new_string->length = 0;
    new_string->capacity = length;
    new_string->raw_string = inner_str;
    return new_string;
}

string_t * make_string(char * raw_string) {
    string_t * new_string = calloc(1, sizeof(string_t));
    if (new_string == NULL)
        return NULL;

    new_string->capacity = sizeof(raw_string);
    new_string->length = sizeof(raw_string);
    new_string->raw_string = raw_string;

    return new_string;
}

static string_t * resize_string(string_t * str) {
    char * new_string = calloc(str->capacity * 2, sizeof(char));
    if (new_string == NULL)
        return NULL;
    strncpy(new_string, str->raw_string, str->length);
    free(str->raw_string);
    str->capacity *= 2;
    str->raw_string = new_string;
    return str;
}

string_t * read_string(const int fd) {
    string_t * buffer = empty_string(BUFFER_SIZE);
    if (buffer == NULL) return NULL;

    char ch;

    for (;;) {
        read(fd, &ch, sizeof(ch));
        if (buffer->capacity == buffer->length) {
            string_t * new_buffer = resize_string(buffer);
            if (new_buffer == NULL) {
                delete_string(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }

        if (ch == '\n') break;
        buffer->raw_string[buffer->length++] = ch;
    }
    return buffer;
}

string_t * shrink_string(const string_t * str){
    if (str == NULL)
        return NULL;
    if (str->raw_string == NULL)
        return NULL;

    const size_t actual_chars = strlen(str->raw_string);
    char * shrinked_string = calloc(actual_chars, sizeof(char));
    
    if (shrinked_string == NULL)
        return NULL;

    string_t * new_string = make_string(shrinked_string);
    
    if (new_string == NULL) {
        free(shrinked_string);
        return NULL;
    }
    return new_string;
}

void delete_string(string_t * str) {
    if (str == NULL) return;
    free(str->raw_string);
    free(str);
}
