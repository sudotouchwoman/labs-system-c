#ifndef _LINKS_H
#define _LINKS_H

#include <regex.h>

#include "common.h"

#define MAX_GROUPS (2)
#define MAX_MATCHES (256)

enum {
    URL_WAIT,
    SOL,
    EOL,
    LINKS_NULL_PTR_ERROR,
    LINKS_OK,
    NON_COMPILED = 0,
    COMPILED
};

int init_parser();
int search_urls(const char * document);

#endif
