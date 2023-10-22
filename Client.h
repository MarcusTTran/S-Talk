// Struct created to encapsulate data needed to send messages 
// to the peer machine.

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
};

char* client_request();


//initializing client object
struct Client client_constructor(int domain, int service, int port, char* hostName);

#endif