#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <netinet/in.h>

#define PROTOCOL_DEFAULT 0

typedef unsigned long u_long;

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

struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port);



#endif