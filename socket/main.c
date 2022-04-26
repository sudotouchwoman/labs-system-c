#include "client.h"
#include "links.h"

int main(int argc, char* argv[]) {

    // char url[] = "http://example.com/";
    // char url[] = "http://fedoruk.comcor.ru/Dev_bach/netprg0.html";
    char url[] = "http://localhost/some-resource.txt";
    const char port[] = "8000";
    char *protocol_removed = NULL;

    const char http[] = "http://";
    const char https[] = "https://";

    if ((strstr(url, http) != NULL)) {
        protocol_removed = url + sizeof(http) - 1;
    }

    if ((strstr(url, https) != NULL)) {
        protocol_removed = url + sizeof(https) - 1;
    }

    const int sock = http_get(
        protocol_removed,
        port);
    if (!connected(sock)) {
        fprintf(stderr, "Failed to connect\n");
        return EXIT_FAILURE;
    }
    http_recieve(sock, fileno(stderr), search_urls);
    return EXIT_SUCCESS;
}
