//trying to push some comment and code

#include "Client.h"
#include <stdlib.h>
#include <string.h>

//can be in client.h
//request for the client 
char* client_request();

//can be in client.h
//initializing client object
struct Client create_client(int domain, char* serverIP, char* client_request);