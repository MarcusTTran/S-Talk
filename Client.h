#ifndef CLIENT_H
#define CLIENT_H

#include <stdlib.h>
#include <string.h>

//request for the client 
char* client_request();


//initializing client object
struct Client create_constructor(int domain, char* serverIP, char* client_request);

#endif