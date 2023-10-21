#include "Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct Server server_constructor(int domain, int service, int protocol, u_long interface, int port){
    struct Server server;

    // initialize member fields of server
    server.domain = domain;
    server.service = service;
    server.protocol = protocol;
    server.interface = interface;
    server.port = port;

    return server;
}