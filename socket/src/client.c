#include "client.h"

#include <arpa/inet.h>  // inet_pton
#include <netdb.h>  // gethostbyname
#include <sys/socket.h>
#include <errno.h>

int is_valid_port(const int port) {
    return MIN_PORT < port && port < MAX_PORT;
}

int is_valid_ip(const char *const ip) {
    struct in_addr addr;
    return inet_pton(AF_INET, ip, &addr) == 1 ? IP_VALID: IP_INVALID;
}

int connected(const int sock) {
    return !(sock == CLIENT_CONN_ERROR || sock == CLIENT_SOCKET_ERROR);
}

static int dns_lookup(const char *const host, char *const ip) {
    struct hostent *entry;
    struct in_addr **addr_list;
    if ((entry = gethostbyname(host)) == NULL) {
        return CLIENT_DNS_LOOKUP_ERROR;
    }
    addr_list = (struct in_addr**) entry->h_addr_list;
    for (size_t i = 0; addr_list[i] != NULL; ++i) {
        strncpy(ip, inet_ntoa(*addr_list[i]), IPv4_SIZE);
        return CLIENT_OK;
    }
    return CLIENT_DNS_LOOKUP_ERROR;
}

static const char GET_REQUEST_HEADER[] =
    "GET %s HTTP/1.1\r\n"
    "HOST: %s\r\n"
    "Connection: close\r\n\r\n"
    ;

static const char GET_ROOT_REQUEST_HEADER[] =
    "GET / HTTP/1.1\r\n"
    "HOST: %s\r\n"
    "Connection: close\r\n\r\n"
    ;

static char FILE_PATH[FILE_BUF];

int http_get(char *url, const char *port) {
    // send GET request to the specified url and port
    char *aux;
    char request_body[BUF_SIZE];
    const char *const url_copy = url;

    if (is_valid_ip(url) == IP_VALID || (aux = strstr(url, "/")) == NULL) {
        // valid IP address/bare hostname without file path is given
        // e.g., "1.1.1.1" or "github.com"
        snprintf(request_body, BUF_SIZE, GET_ROOT_REQUEST_HEADER, url);
    } else {
        // host name contains a slash --> it is a path to file (resource)
        // e.g., "github.com/sudotouchwoman"
        strncpy(FILE_PATH, aux, FILE_BUF);
        strtok(url, "/");
        snprintf(request_body, BUF_SIZE, GET_REQUEST_HEADER, FILE_PATH, url);
    }

    fprintf(stderr, "Formed request:\n%s", request_body);

    char ip[IPv4_SIZE];
    memset(ip, 0, IPv4_SIZE);

    if (dns_lookup(url_copy, ip) != CLIENT_OK) {
        fprintf(stderr, "DNS lookup failed! Host: %s\n", url_copy);
        return CLIENT_DNS_LOOKUP_ERROR;
    }

    fprintf(stderr, "Resolved IP: %s\n", ip);

    // create a socket to the host
    const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "Error creating socket!\n");
        return CLIENT_SOCKET_ERROR;
    }

    fprintf(stderr, "Created socket...\n");

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(atoi(port));

    // try to connect to the host
    if (connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "Connection Error!\n");
        return CLIENT_CONN_ERROR;
    }

    fprintf(stderr, "Connection successful, sending GET request...\n\n");
    write(sockfd, request_body, strnlen(request_body, BUF_SIZE));
    return sockfd;
}

static ssize_t written(
    const int fd,
    const char *buf,
    const size_t s,
    const int retry_on_interrupt
) {
    size_t bytes_left = s;
    while (bytes_left > 0) {
        ssize_t wrote_bytes = write(fd, buf, bytes_left);
        if (wrote_bytes == -1) {
            if (
                (retry_on_interrupt && (errno == EINTR)) ||
                (errno == EWOULDBLOCK) ||
                (errno == EAGAIN)
            ) { continue; }
            else { break; }
        }

        bytes_left -= wrote_bytes;
        buf += wrote_bytes;
    }

    return bytes_left;
}

int http_recieve(
    const int sockfd,
    const int outfd
) {
    static char RESPONSE_BUFFER[BUF_SIZE];
    memset(&RESPONSE_BUFFER, 0, BUF_SIZE);

    int ret = 0;
    ssize_t bytes_read = 0;
    ssize_t bytes_left = 0;

    while ((ret = recv(sockfd, RESPONSE_BUFFER, BUF_SIZE, 0)) > 0) {
        bytes_read += ret;
        bytes_left += written(outfd, RESPONSE_BUFFER, ret, 0);
        memset(&RESPONSE_BUFFER, 0, BUF_SIZE);
    }

    fprintf(stderr, "Socket: Read %li bytes, wrote %li bytes\n\n\n", bytes_read, bytes_read - bytes_left);
    return 0;    
}

int url_contains_protocol(const char *const url) {
    return (strstr(url, "://") != NULL);
}
