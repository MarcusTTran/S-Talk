#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
// Networking includes
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

// My own files includes
#include "list.h"
#include "Server.h"
#include "Client.h"
#include "input.c"
// #include "output.c"

// CSIL MACHINE IP: 127.0.0.1
#define PORT 6969
#define REMOTE_NAME_BUFFER 25 
// #define MSG_MAX_LENGTH 256
#define FLAGS_DEFAULT 0

// Will hold the args from command line
int myPortNum, destPortNum;
char destName[REMOTE_NAME_BUFFER];

// Function headers
void initTalkArgs(int argc, char *argv[]);
// void * runServer(void * arg);
void setupAndReceiveMessage();
int replyToSender();



int main(int argc, char *argv[]) {
   initTalkArgs(argc, argv);
//    pthread_t server_thread;
//    pthread_create(server_thread, NULL, runServer, NULL);
    setupAndReceiveMessage();
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

// void * runServer(void * arg) {

//     struct Server myServer = server_constructor(AF_INET, SOCK_DGRAM, 0, INADDR_ANY, myPortNum);


//     return NULL;
// }

// Using this function to test to see if basic s-talk works
void setupAndReceiveMessage() {
    // Setup my socket(will be server) to receive msg from peer
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin)); 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(myPortNum); 
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // TODO: change this to the IP address of our computer

    int mySocketDescriptor = socket(AF_INET, SOCK_DGRAM, PROTOCOL_DEFAULT);
    bind(mySocketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    // Setup for getaddrinfo()
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    
    struct addrinfo* destInfoResults;
    char portStr[10];
    sprintf(portStr, "%d", destPortNum); // get the destination port number in a string

    int statusOfAddrInfo = getaddrinfo(destName, portStr, &hints, &destInfoResults);
    if (statusOfAddrInfo != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(statusOfAddrInfo));
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in * sinRemote = (struct sockaddr_in*)destInfoResults->ai_addr;
    unsigned int sinRemote_len = sizeof(struct sockaddr_in);
   
    // Receive message (like a server)
    char messageRx[MSG_MAX_LENGTH];
    // Blocking call to wait for msg to be received
    int bytesRx = recvfrom(mySocketDescriptor, messageRx, MSG_MAX_LENGTH, 0,
                    (struct sockaddr*) &sinRemote, &sinRemote_len); 
    if (bytesRx == -1) { 
        printf("Error in receiving message! Exiting program");
        close(mySocketDescriptor);
        freeaddrinfo(destInfoResults);
        exit(0); 
    }
    // Ensure it is a null terminated string
    int terminateMessageIndex = (bytesRx < MSG_MAX_LENGTH) ? bytesRx : MSG_MAX_LENGTH - 1;
    messageRx[terminateMessageIndex] = 0;
    // Print it out to console
    printf("Message received(%d bytes): '%s'\n", bytesRx, messageRx);

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

