#define _GNU_SOURCE
#include "Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port){
    struct Server server;

    // initialize member fields of server
    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;

    server.address.sin_family = domain; 
    server.address.sin_port = htons(port);

    struct addrinfo hints;


    server.address.sin_addr.s_addr = htonl(INADDR_ANY); // TODO: change this to the IP address of our computer

    return server;
}