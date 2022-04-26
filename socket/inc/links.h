#ifndef _LINKS_H
#define _LINKS_H

#include <regex.h>

#include "common.h"

// typedef struct {
//     char * url;
//     char * host;
//     char * resource;
// } url_t;

// typedef struct {
//     node_t* next;
//     url_t* data;
// } node_t;

// typedef struct {
//     node_t* root;
//     size_t len;
// } url_list_t;

// node_t *make_node(url_t *const url);

int search_urls(const char * document);

#endif
