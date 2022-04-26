#include "links.h"

static size_t calls = 0;

int search_urls(const char * document) {
    fprintf(stdout, "\n>>> [%lu] URL parser called\n", ++calls);
    return 0;
}
