#ifndef _LINKS_H
#define _LINKS_H

#include <regex.h>

#include "common.h"

#define MAX_GROUPS (2)
#define MAX_MATCHES (256)

#define RESPONSE_FILE ".response.tmp"
#define URLS_FILE ".urls.tmp"

void parse();
size_t list_urls();
const char *const pick_next_url(const size_t url_num);
size_t url_choice(const size_t url_count, int *const status);

#endif
