#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
// Networking includes
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
// My own files includes
#include "list.h"

// CSIL MACHINE IP: 127.0.0.1
// #define PORT 6969
#define REMOTE_NAME_BUFFER 25 

// Will hold the args from command line
int myPortNum, destPortNum;
char destName[REMOTE_NAME_BUFFER];

// Function headers
void initTalkArgs(int argc, char *argv[]);
void * runServer(void * arg);
void createUDPSocket();


int main(int argc, char *argv[]) {
   initTalkArgs(argc, argv);
   pthread_t server_thread;
   pthread_create(server_thread, NULL, runServer, NULL);


}

void initTalkArgs(int argc, char *argv[]) {
    myPortNum = atoi(argv[1]);
    if (strlen(argv[2]) < REMOTE_NAME_BUFFER) {
        strcpy(destName, argv[2]);
    } else {
        printf("Invalid remote machine name. Please try again.\n");
        exit(0);
    }
    destPortNum = atoi(argv[3]);
}

void * runServer(void * arg) {
    struct Server = server_constructor(AF_INET, SOCK_DGRAM, );
    return NULL;
}


// using get getaddrinfo()
void createUDPSocket() {

    struct addrinfo hints, *serverinfo, *print; // res and print will be used as **pointers
    int addrinfo_status; // 0 if failed; otherwise success
    
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;

    if ((addrinfo_status = getaddrinfo(NULL, "3333", &hints, &serverinfo)) != 0)
    // // Print the information out
    // char ipstr[INET6_ADDRSTRLEN];


    freeaddrinfo(serverinfo); // free the linked-list
}

