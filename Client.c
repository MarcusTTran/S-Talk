#define _GNU_SOURCE
#include "Client.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


char* client_request();

//initializing client object
struct Client client_constructor(int domain, int service, int port, char* hostName, int destPort) {
    struct Client client;

    client.domain = domain;
    client.service = service;
    client.port = port;
    client.hostName = hostName;

    // For obtaining address information of the our machine
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    char portStr[10];
    sprintf(portStr, "%d", port); // get our port number in a string
    struct addrinfo* myInfoResults;

    int statusOfAddrInfo = getaddrinfo(NULL, portStr, &hints, &myInfoResults);
    if (statusOfAddrInfo != 0) {
        // fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(statusOfAddrInfo));
        printf("Error in client constructor (client): getaddrinfo\n");
        exit(EXIT_FAILURE);
    }

    client.interface = ((struct sockaddr_in*)myInfoResults->ai_addr)->sin_addr.s_addr;
    client.address = *(struct sockaddr_in*)myInfoResults->ai_addr;
    
    // Create socket with the obtained info
    client.socket = socket(client.domain, client.service, PROTOCOL_DEFAULT);
    if ( bind(client.socket, (struct sockaddr*) &client.address, sizeof(client.address)) 
        != 0) {
        printf("Bind to socket failed in client constructor\n");
        exit(EXIT_FAILURE);
    }

    // Setup info for destination the client will be sending to
    client.destPort = destPort;
    char destPortStr[10];
    sprintf(destPortStr, "%d", destPort); // get the destination port number in a string

    struct addrinfo * destInfoResults;
    statusOfAddrInfo = getaddrinfo(hostName, destPortStr, &hints, &destInfoResults);
    if (statusOfAddrInfo != 0) {
        // fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(statusOfAddrInfo));
        printf("Error in client constructor (remote): getaddrinfo\n");
        exit(EXIT_FAILURE);
    }

    client.sendToAddr = (struct sockaddr_in*)destInfoResults->ai_addr;

    freeaddrinfo(destInfoResults);
    freeaddrinfo(myInfoResults);
    return client;
}

void closeClient(struct Client client) {
    close(client.socket);
}