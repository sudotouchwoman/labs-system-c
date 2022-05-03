#include "client.h"
#include "urls.h"
#include "iotty.h"

int main(int argc, char* argv[]) {

    char url[BUF_SIZE];
    char port[PORT_BUF];

    memset(url, 0, BUF_SIZE);
    memset(port, 0, PORT_BUF);
    strncpy(port, "80", PORT_BUF - 1);

    // if (argc == 1) {
    //     usage(fileno(stdout));
    //     return EXIT_FAILURE;
    // }

    // if (argc > 1) {
    //     strncpy(url, argv[1], BUF_SIZE - 1);
    //     if (argc == 3)
    //         strncpy(port, argv[2], PORT_BUF - 1);

    //     if (argc > 3) {
    //         usage(fileno(stdout));
    //         return EXIT_FAILURE;
    //     }
    // }

    // char url[] = "http://example.com/";
    // char url[] = "http://fedoruk.comcor.ru/non_canon.html";
    // char url[] = "http://localhost/some-resource.txt";
    // const char port[] = "80";
    char *protocol_removed = url;

    const char http[] = "http://";
    const char https[] = "https://";

    for (;;) {
        fprintf(stderr, "Press Q to quit, Enter to enter website location: ");
        char key_pressed = 0;
        read(fileno(stdin), &key_pressed, 1);

        if (key_pressed == 'Q' || key_pressed == 'q') {
            fprintf(stdout, "Quit...\n");
            break;
        }

        if (key_pressed == 10) {
            fprintf(stdout, "Enter new URL: ");
            if ((fscanf(stdin, "%s", url)) != 1) {
                fprintf(stderr, "Error reading URL\n");
                continue;
            };
        } else {
            fprintf(stderr, "Invalid option: %c\n", key_pressed);
            continue;
        }
        init_parser();
        if ((strstr(url, http) != NULL)) {
            protocol_removed = url + sizeof(http) - 1;
        }
        if ((strstr(url, https) != NULL)) {
            protocol_removed = url + sizeof(https) - 1;
        }

        fprintf(stdout, "Connecting to %s\n", url);
        const int sock = http_get(protocol_removed, port);

        if (!connected(sock)) {
            fprintf(stderr, "Failed to connect\n");
            continue;
        }

        // recieve the response and pass it to the parser
        http_recieve(sock, fileno(stderr), search_urls);
        fprintf(stderr, "Finished with url %s\n", url);
        memset(url, 0, BUF_SIZE);
        memset(port, 0, PORT_BUF);
        strncpy(port, "80", PORT_BUF - 1);
    }

    return EXIT_SUCCESS;
}
