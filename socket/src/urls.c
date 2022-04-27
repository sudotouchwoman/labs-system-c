#include "urls.h"

static size_t urls_found = 0;
static size_t calls = 0;
static int parser_state = URL_WAIT;
static int compiled = NON_COMPILED;

static int init_parser() {
    urls_found = 0;
    parser_state = URL_WAIT;
    return urls_found;
}

static const char A_FULL_MATCH[] =
    "(<a[[:blank:][:alnum:]=\"]* href=\"([^\"]*)[[:blank:][:alnum:]=\"]*\">)";

// static const char A_START_MATCH[] =
//     "";

static regex_t pattern;

int compile_re() {
    fprintf(stderr, "Compiles regex: %s\n", A_FULL_MATCH);
    return regcomp(&pattern, A_FULL_MATCH, REG_EXTENDED);
}

static int parse_chunk(char *const chunk) {
    if (chunk == NULL) return LINKS_NULL_PTR_ERROR;

    char *cursor = chunk;
    regmatch_t group_array[MAX_GROUPS];
    memset(group_array, 0, sizeof(group_array));

    fprintf(stderr, ">>> START OF CHUNK!!!\n\n%s\n\n>>> END OF CHUNK !!!\n", chunk);

    for (size_t i = 0; i < MAX_MATCHES; ++i) {
        if (regexec(&pattern, cursor, MAX_GROUPS, group_array, 0)) break;

        fprintf(stderr, "\n>>> FOUND SOMETHING IN CHUNK\n");
        size_t gi = 0;
        size_t offset = 0;

        for (; gi < MAX_GROUPS; ++gi) {
            if (group_array[gi].rm_so == (size_t)-1)
                break;  // no more groups

            if (gi == 0)
                offset = group_array[gi].rm_eo;

            char cursor_copy[strlen(cursor) + 1];
            strcpy(cursor_copy, cursor);
            cursor_copy[group_array[gi].rm_eo] = 0;

            fprintf(stderr, "\n>>> Match %lu Group %lu: [%u-%u]: %s\n",
                i, gi, group_array[gi].rm_so, group_array[gi].rm_eo,
                cursor_copy + group_array[gi].rm_so);
        }
        cursor += offset;
        ++urls_found;
    }

    regfree(&pattern);    
    return 0;
}

int search_urls(const char * document) {
    fprintf(stdout, "\n>>> [%lu] URL parser called\n", ++calls);
    if (compiled == NON_COMPILED) {
        const int status = compile_re();
        fprintf(stderr, "%s: %d\n", status? "Did not compile!": "Compiled, OK", status);
        if (status) return -1;
        compiled = COMPILED;
    }
    init_parser();
    return parse_chunk((char*)document);
}
