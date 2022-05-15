#include "urls.h"

static const char GREP_CALL[] = 
    "cat " RESPONSE_FILE " | "
    "grep -Eo '<a [^>]+>' |"
    "grep -Eo 'href=\"[^\"]+\"' | "
    "cut -c 7- | "
    "rev | "
    "cut -c 2- | "
    "rev "
    " > " URLS_FILE
    ;

static char BUFFER[BUF_SIZE];

void parse() {
    // use grep to parse the obtained http response
    FILE *const response = fopen(RESPONSE_FILE, "r");

    int ret = 0;
    memset(BUFFER, 0, BUF_SIZE);

    while ((ret = read(fileno(response), BUFFER, BUF_SIZE)) > 0) {
        write(fileno(stdout), BUFFER, ret);
        memset(BUFFER, 0, BUF_SIZE);
    }

    // use grep command to search for URLs
    // and store the result in a temporary file
    system(GREP_CALL);
    fclose(response);
}

size_t list_urls() {
    // list all the URLs found on the page
    // by reading grep's output
    FILE *const urls = fopen(URLS_FILE, "r");
    memset(BUFFER, 0, BUF_SIZE);

    fprintf(stderr, "\nFOUND LINKS:\n");

    // print the results of grep to the terminal
    size_t url_id = 0;
    while (fgets(BUFFER, BUF_SIZE - 1, urls) != NULL) {
        fprintf(stderr, "%2.lu: ", ++url_id);
        write(fileno(stderr), BUFFER, strnlen(BUFFER, BUF_SIZE));
        memset(BUFFER, 0, BUF_SIZE);
    }

    fclose(urls);
    return url_id;
}

size_t url_choice(const size_t url_count, int *const ok) {
    // let the user select number of URL to open next
    fprintf(stderr, "Enter id of URL to open (not greater than %lu), 0 to return to the previous page or -1 to abort: ", url_count);
    size_t picked = 0;
    if (fscanf(stdin, "%lu", &picked) == 1 && ok != NULL)
        *ok = 1;
    if (picked > url_count && ok != NULL)
        *ok = 0;
    return picked;
}

const char *const pick_next_url(const size_t url_num) {
    // read URLs from tmp file until
    // the given number of lines is read
    // or EOF encountered
    FILE *const urls = fopen(URLS_FILE, "r");
    size_t url_id = 1;
    memset(BUFFER, 0, BUF_SIZE);

    while (fgets(BUFFER, BUF_SIZE - 1, urls) != NULL && ++url_id <= url_num) {
        memset(BUFFER, 0, BUF_SIZE);
    }

    fclose(urls);
    return BUFFER;
}
