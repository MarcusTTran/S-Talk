#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

#define PROTOCOL_DEFAULT 0

typedef unsigned long u_long;

struct Server {
    int domain;
    int service;
    int protocol;
    u_long interface;
    int port;
    struct sockaddr_in address;
    int socket;

};

struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port);



#endif