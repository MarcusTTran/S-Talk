#define _GNU_SOURCE
#include "Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define PORT_LEN 10

struct Server server_constructor(int domain, int service, int protocol, int port){
    struct Server server;

    // initialize member fields of server
    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.port = port;

    // Setup address field to open up socket later
    server.address.sin_family = domain; 
    server.address.sin_port = htons(port);

    // Get address of local machine (my server)
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    char portStr[PORT_LEN];
    sprintf(portStr, "%d", port); 

    struct addrinfo* addrInfoResults;
    int result = getaddrinfo(NULL, portStr, &hints, &addrInfoResults);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(addrInfoResults));
        exit(EXIT_FAILURE);
    }
    server.interface = ((struct sockaddr_in*)addrInfoResults->ai_addr)->sin_addr.s_addr;
    server.address = *(struct sockaddr_in*)addrInfoResults->ai_addr; 

    // Create socket with the obtained info
    server.socket = socket(AF_INET, SOCK_DGRAM, PROTOCOL_DEFAULT);
    if ( bind(server.socket, (struct sockaddr*) &server.address, sizeof(server.address)) 
        != 0) {
        printf("Bind to socket failed in server constructor\n");
        exit(EXIT_FAILURE);
    }

    return server;
}