#include "urls.h"

// static const char A_FULL_MATCH[] =
//     "(<a[[:blank:][:alnum:]=\"]* href=\"([^\"]*)[[:blank:][:alnum:]=\"]*\">)";

// static regex_t pattern;

// int compile_re() {
//     fprintf(stderr, "Compiles regex: %s\n", A_FULL_MATCH);
//     return regcomp(&pattern, A_FULL_MATCH, REG_EXTENDED);
// }

// static int parse_chunk(char *const chunk) {
//     if (chunk == NULL) return LINKS_NULL_PTR_ERROR;

//     char *cursor = chunk;
//     regmatch_t group_array[MAX_GROUPS];
//     memset(group_array, 0, sizeof(group_array));

//     fprintf(stderr, ">>> START OF CHUNK!!!\n\n%s\n\n>>> END OF CHUNK !!!\n", chunk);

//     for (size_t i = 0; i < MAX_MATCHES; ++i) {
//         if (regexec(&pattern, cursor, MAX_GROUPS, group_array, 0)) break;

//         fprintf(stderr, "\n>>> FOUND SOMETHING IN CHUNK\n");
//         size_t gi = 0;
//         size_t offset = 0;

//         for (; gi < MAX_GROUPS; ++gi) {
//             if (group_array[gi].rm_so == (size_t)-1)
//                 break;  // no more groups

//             if (gi == 0)
//                 offset = group_array[gi].rm_eo;

//             char cursor_copy[strlen(cursor) + 1];
//             strcpy(cursor_copy, cursor);
//             cursor_copy[group_array[gi].rm_eo] = 0;

//             fprintf(stderr, "\n>>> Match %lu Group %lu: [%u-%u]: %s\n",
//                 i, gi, group_array[gi].rm_so, group_array[gi].rm_eo,
//                 cursor_copy + group_array[gi].rm_so);
//         }
//         cursor += offset;
//         ++urls_found;
//     }

//     regfree(&pattern);
//     return 0;
// }

static const char GREP_CALL[] = 
    "cat " RESPONSE_FILE
    " | grep -Eo \"https?://[a-zA-Z0-9./?=_%:-]*\""
    " > " URLS_FILE
    ;

// static char URL_BUF[BUF_SIZE];

void parse() {
    FILE *const response = fopen(RESPONSE_FILE, "r");
    FILE *const urls = fopen(URLS_FILE, "r");

    int ret = 0;
    char BUFFER[BUF_SIZE];
    memset(BUFFER, 0, BUF_SIZE);

    while ((ret = read(fileno(response), BUFFER, BUF_SIZE)) > 0) {
        write(fileno(stdout), BUFFER, ret);
        memset(BUFFER, 0, BUF_SIZE);
    }

    // FILE *const grep = popen(GREP_CALL, "r");
    // if (grep == NULL) {
    //     fprintf(stderr, "Failed to call grep\n");
    //     exit(EXIT_FAILURE);
    // }
    system(GREP_CALL);

    fprintf(stderr, "GREP OUTPUTS\n");

    while (fgets(BUFFER, BUF_SIZE - 1, urls) != NULL) {
        write(fileno(stderr), BUFFER, strnlen(BUFFER, BUF_SIZE));
        memset(BUFFER, 0, BUF_SIZE);
    }

    // pclose(grep);
    fclose(response);
    fclose(urls);
    fprintf(stderr, "GREP FINISHED\n");
}

// int search_urls(const char * document) {
//     fprintf(stdout, "\n>>> [%lu] URL parser called\n", ++calls);
//     compile_re();
//     return parse_chunk((char*)document);
// }
