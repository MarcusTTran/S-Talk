#ifndef SERVER_H
#define SERVER_H

#include "headers.h"

struct Server {
    // For the socket
    int domain;
    int service;
    int protocol;
    int socket;

    // for ip address
    u_long interface;
    int port;
    struct sockaddr_in address;

};

struct Server server_constructor(int domain, int service, int protocol, int port);
void closeServer(struct Server server);
#endif