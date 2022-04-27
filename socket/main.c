#include "client.h"
#include "urls.h"
#include "iotty.h"

int main(int argc, char* argv[]) {

    char url[BUF_SIZE];
    char port[PORT_BUF];

    memset(url, 0, BUF_SIZE);
    memset(port, 0, PORT_BUF);
    strncpy(port, "80", PORT_BUF - 1);

    if (argc == 1) {
        usage(fileno(stdout));
        return EXIT_FAILURE;
    }

    if (argc > 1) {
        strncpy(url, argv[1], BUF_SIZE - 1);
        if (argc == 3)
            strncpy(port, argv[2], PORT_BUF - 1);
        
        if (argc > 3) {
            usage(fileno(stdout));
            return EXIT_FAILURE;
        }
    }

    // char url[] = "http://example.com/";
    // char url[] = "http://fedoruk.comcor.ru/non_canon.html";
    // char url[] = "http://localhost/some-resource.txt";
    // const char port[] = "80";
    char *protocol_removed = NULL;

    const char http[] = "http://";
    const char https[] = "https://";

    if ((strstr(url, http) != NULL)) {
        protocol_removed = url + sizeof(http) - 1;
    }

    if ((strstr(url, https) != NULL)) {
        protocol_removed = url + sizeof(https) - 1;
    }

    const int sock = http_get(protocol_removed, port);

    if (!connected(sock)) {
        fprintf(stderr, "Failed to connect\n");
        return EXIT_FAILURE;
    }

    http_recieve(sock, fileno(stderr), search_urls);
    return EXIT_SUCCESS;
}
