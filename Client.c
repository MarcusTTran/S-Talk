#define _GNU_SOURCE
#include "Client.h"
#include <string.h>


char* client_request();

//initializing client object
struct Client client_constructor(int domain, int service, int port, char* hostName) {
    struct Client client;

    client.domain = domain;
    client.service = service;
    client.port = port;
    client.hostName = hostName;

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;




    client.interface
    client.address
    

}