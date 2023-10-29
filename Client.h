// Struct created to encapsulate data needed to send messages 
// to the peer machine. Uses the same socket as in the Server struct.

#ifndef CLIENT_H
#define CLIENT_H

#include "headers.h"

struct Client {
     // For the socket
    int domain;
    int service;
    int socket;

    // for ip address
    u_long interface;
    int port;
    struct sockaddr_in address;
    char* hostName;

    // for destination
    int destPort;
    struct sockaddr_in * sendToAddr;

    // for getaddrinfo results
    struct addrinfo * myInfoResultsGlobal;
    struct addrinfo * destInfoResutlsGlobal;
};



//initializing client object
struct Client client_constructor(int domain, int service, int port, char* hostName, int destPort);

void closeClient(struct Client client);
#endif