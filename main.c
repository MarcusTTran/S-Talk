#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// My own files includes
#include "list.h"
#include "Server.h"
#include "Client.h"
#include "input.c"
#include "output.c"
#include "headers.h"

// CSIL MACHINE IP: 127.0.0.1
#define PORT 6969
#define REMOTE_NAME_BUFFER 25 
// #define MSG_MAX_LENGTH 256


// Function headers
void initTalkArgs(int argc, char *argv[]);
void * runServer(void * arg);
// void setupAndReceiveMessage();
int replyToSender();
void terminateProgram();

// Will hold the args from command line
int myPortNum, destPortNum;
char destName[REMOTE_NAME_BUFFER];

// GLOBAL Variables
struct Server serverRx;
struct Client clientTx;


int main(int argc, char *argv[]) {
    initTalkArgs(argc, argv);
    serverRx = server_constructor(AF_INET, SOCK_DGRAM, PROTOCOL_DEFAULT, myPortNum);
    clientTx = client_constructor(AF_INET, SOCK_DGRAM, myPortNum, destName, destPortNum);

    pthread_t server_thread;
    pthread_create(&server_thread, NULL, runServer, NULL);

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
    u_long sinRemoteLen = sizeof(clientTx.sendToAddr);
    char messageRx[MSG_MAX_LENGTH];
    int bytesRx = 0;
    int terminateMessageIndex = 0;
    // Wait and receive message from remote connection through user specified port
    while(1) {
         bytesRx = recvfrom(serverRx.socket, messageRx, MSG_MAX_LENGTH, FLAGS_DEFAULT, 
                    (struct sockaddr*) &clientTx.sendToAddr, &sinRemoteLen);
        if (bytesRx == -1) { 
            printf("Error in receiving message!\n");
            terminateProgram(serverRx, clientTx);
        }
        // ensure string is null terminated
        terminateMessageIndex = (bytesRx < MSG_MAX_LENGTH) ? bytesRx : MSG_MAX_LENGTH - 1;
        messageRx[terminateMessageIndex] = 0;
        // Print it out to console
        printf("Message received(%d bytes): '%s'\n", bytesRx, messageRx);
        // TODO: add it to a shared list
    }
    
    return NULL;
}

void * runClient(void * arg) {
    
    return NULL;
}

// Using this function to test to see if basic s-talk works
void setupAndReceiveMessage() {
    // // Setup my socket(will be server) to receive msg from peer
    // struct sockaddr_in sin;
    // memset(&sin, 0, sizeof(sin)); 
    // sin.sin_family = AF_INET;
    // sin.sin_port = htons(myPortNum); 
    // sin.sin_addr.s_addr = htonl(INADDR_ANY); 

    // int mySocketDescriptor = socket(AF_INET, SOCK_DGRAM, PROTOCOL_DEFAULT);
    // bind(mySocketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    // Setup for getaddrinfo()
    // struct addrinfo hints;
    // memset(&hints, 0, sizeof(hints));
    // hints.ai_family = AF_INET;
    // hints.ai_socktype = SOCK_DGRAM;
    
    // struct addrinfo* destInfoResults;
    // char portStr[10];
    // sprintf(portStr, "%d", destPortNum); // get the destination port number in a string

    // int statusOfAddrInfo = getaddrinfo(destName, portStr, &hints, &destInfoResults);
    // if (statusOfAddrInfo != 0) {
    //     fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(statusOfAddrInfo));
    //     exit(EXIT_FAILURE);
    // }

    // struct sockaddr_in * sinRemote = (struct sockaddr_in*)destInfoResults->ai_addr;
    
    // unsigned int sinRemote_len = sizeof(struct sockaddr_in);
   
    // Receive message (like a server)
    // char messageRx[MSG_MAX_LENGTH];
    // // Blocking call to wait for msg to be received
    // int bytesRx = recvfrom(mySocketDescriptor, messageRx, MSG_MAX_LENGTH, 0,
    //                 (struct sockaddr*) &sinRemote, &sinRemote_len); 
    // if (bytesRx == -1) { 
    //     printf("Error in receiving message! Exiting program");
    //     terminateMessageIndex(serverRx, clientTx);
    // }
    // // Ensure it is a null terminated string
    // int terminateMessageIndex = (bytesRx < MSG_MAX_LENGTH) ? bytesRx : MSG_MAX_LENGTH - 1;
    // messageRx[terminateMessageIndex] = 0;
    // // Print it out to console
    // printf("Message received(%d bytes): '%s'\n", bytesRx, messageRx);

    // Send a message back (Reply)
    char replyTx[MSG_MAX_LENGTH] = "Your message was received!\n";
    // sprintf(replyTx, "Your message was received: \n(%s)\n", messageRx);
    // int replyResult = replyToSender(replyTx, mySocketDescriptor, sinRemote);
    replyToSender(replyTx, mySocketDescriptor, sinRemote);
    
    close(mySocketDescriptor);
    freeaddrinfo(destInfoResults);
}   

// Sends a reply in the form of a char[] to a sender name sinRemote using a user-specified socket
int replyToSender(const char message[], int mySocket, struct sockaddr_in * sinRemote) { 
    socklen_t sinRemoteLen = sizeof(*sinRemote);
    int bytesTx = sendto(mySocket, message, strlen(message), FLAGS_DEFAULT, 
        (struct sockaddr*) &sinRemote, sinRemoteLen);
    if (bytesTx == -1) {
        printf("Error occured in replying!\n");
        return -1;
    }
    return 0;
}

// In the case of a major bug or error, call this to close free structs and 
// terminate the program
void terminateProgram(struct Server server, struct Client client) {
    closeClient(client);
    closeServer(server);
    exit(EXIT_FAILURE);
}

