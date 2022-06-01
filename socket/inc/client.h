#ifndef _CLIENT_H
#define _CLIENT_H

#include "common.h"

#define IPv4_SIZE (3*4+4)

enum {
    IP_VALID,
    IP_INVALID,
    CLIENT_OK = 0,
    CLIENT_SOCKET_ERROR = -2,
    CLIENT_CONN_ERROR,
    CLIENT_DNS_LOOKUP_ERROR,
    MAX_PORT = 65536,
    MIN_PORT = 0
};

int is_valid_port(const int port);
int is_valid_ip(const char *const ip);
int connected(const int sockfd);
int http_get(char * url, const char * port);
int http_recieve(const int sockfd, const int outfd);
int url_contains_protocol(const char *const url);

#endif
