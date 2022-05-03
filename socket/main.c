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

    int URL_LOADED = 0;

    // infinite loop to wait for user input and scrape URLs
    for (;;) {
        if (!URL_LOADED) {
            memset(url, 0, BUF_SIZE);
            memset(port, 0, PORT_BUF);
            strncpy(port, "80", PORT_BUF - 1);

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
        }

        if (url == NULL || *url == 0) {
            fprintf(stderr, "Empty URL pointer given\n");
            break;
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
            URL_LOADED = 0;
            continue;
        }

        // recieve the response and pass it to the parser
        http_recieve(sock, fileno(response_file));
        // grep response and store the result in tmp file
        parse();

        fclose(response_file);
        fclose(urls_file);

        // let the user choose whether to move somewhere or not
        int ok = 0; URL_LOADED = 0;
        const size_t url_count = list_urls();

        if (!url_count) {
            fprintf(stderr, "No URLs were found on this page\n");
            continue;
        }

        size_t selected_url = url_choice(url_count, &ok);

        if (!ok) {
            fprintf(stderr, "Invalid option.\n");
            continue;
        }

        URL_LOADED = 1;
        const char *new_url = pick_next_url(selected_url);
        if (!url_contains_protocol(new_url)) {
            // must be an addition to the absolute path
            fprintf(stdout, "URL must be relative: %s\n", new_url);
            strcat(url, "/");
            strcat(url, new_url);
            fprintf(stdout, "Cat string: %s\n", url);
            continue;
        }
        // must be a valid http link
        strncpy(url, new_url, sizeof(url) - 1);
    }

    // close temporary files
    remove(RESPONSE_FILE);
    remove(URLS_FILE);
    return EXIT_SUCCESS;
}
