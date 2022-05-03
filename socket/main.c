#include "client.h"
#include "urls.h"
#include "iotty.h"

int main(int argc, char* argv[]) {

    char url[BUF_SIZE];
    char port[PORT_BUF];

    memset(url, 0, BUF_SIZE);
    memset(port, 0, PORT_BUF);
    strncpy(port, "80", PORT_BUF - 1);

    char *protocol_removed = url;

    const char http[] = "http://";
    const char https[] = "https://";

    // infinite loop to wait for user input and scrape URLs
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

        // open temporary files
        FILE *const response_file = fopen(RESPONSE_FILE, "w+");
        FILE *const urls_file = fopen(URLS_FILE, "w+");

        if (response_file == NULL || urls_file == NULL) {
            fclose(response_file);
            fclose(urls_file);
            fprintf(stderr, "Failed to open temporary files\n");
            return EXIT_FAILURE;
        }

        // initialize the parser and remove protocol from url if present
        // init_parser();
        if ((strstr(url, http) != NULL)) {
            protocol_removed = url + sizeof(http) - 1;
        }
        if ((strstr(url, https) != NULL)) {
            protocol_removed = url + sizeof(https) - 1;
        }

        // try to connect to the given URL
        fprintf(stdout, "Connecting to %s\n", url);
        const int sock = http_get(protocol_removed, port);

        if (!connected(sock)) {
            fprintf(stderr, "Failed to connect\n");
            continue;
        }

        // recieve the response and pass it to the parser
        http_recieve(sock, fileno(response_file));
        fprintf(stderr, "Finished with url %s\n", url);
        parse();
        memset(url, 0, BUF_SIZE);
        memset(port, 0, PORT_BUF);
        strncpy(port, "80", PORT_BUF - 1);

        fclose(response_file);
        fclose(urls_file);
    }

    // close temporary files
    remove(RESPONSE_FILE);
    remove(URLS_FILE);
    return EXIT_SUCCESS;
}
