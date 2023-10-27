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
#define MSG_MAX_LENGTH 256


// Function headers
void initTalkArgs(int argc, char *argv[]);
void * runServer(void * arg);
void * runClient();
// void setupAndReceiveMessage();
int replyToSender();
void terminateProgram();

// Will hold the args from command line
int myPortNum, destPortNum;
char destName[REMOTE_NAME_BUFFER];

// GLOBAL Variables
struct Server serverRx;
struct Client clientTx;


void* testThreads(void* arg)
{
    char* msg = arg;
    for(int i = 0; i< strlen(msg); i++)
    {
        printf("%c", msg[i]);
    }
    return NULL;
}


int main(int argc, char *argv[]) {
    printf("Starting...\n");
    initTalkArgs(argc, argv);
//     pthread_t threadPID;
//     pthread_create(&threadPID, NULL, testThreads, "123\n");

//     // printf("Enter '!' to kill the thread\n");
//     // char x;
//     // scanf("%c", &x);

//     // pthread_t threadPID2;
//     // pthread_create(&threadPID2, NULL, testThreads, "321\n");

//     pthread_join(threadPID, NULL);
    
//     setupAndReceiveMessage();

//     // pthread_join(threadPID2, NULL);
//     // testThreads(NULL);
//     // struct Server serverRx;
//     // struct Client clientTx;
//     // initTalkArgs(argc, argv);
//     // createServer(serverRx);
//     // createClient(clientTx);
// //    pthread_t server_thread;
// //    pthread_create(server_thread, NULL, runServer, NULL);

       
    clientTx = client_constructor(AF_INET, SOCK_DGRAM, myPortNum, destName, destPortNum);
    
    char str2[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientTx.sendToAddr), str2, INET_ADDRSTRLEN);
    printf("destination socket address: %s\n", str2);
    printf("destination socket port: %d\n", ntohs(clientTx.destPort));
    
    serverRx = server_constructor(AF_INET, SOCK_DGRAM, PROTOCOL_DEFAULT, myPortNum);
    
    char str1[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serverRx.address), str1, INET_ADDRSTRLEN);
    printf("our socket address: %s\n", str1);
    printf("our socket port: %d\n", ntohs(serverRx.port));
    


    pthread_t server_thread;
    pthread_create(&server_thread, NULL, runServer, NULL);
    

    pthread_t client_thread;
    pthread_create(&client_thread, NULL, runClient, NULL);
    
    pthread_join(server_thread, NULL);
    pthread_join(client_thread, NULL);
    // runClient();

    printf("---DONE---\n");
    return 0;
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

    printf("    our port: %d\n", myPortNum);
    printf("    destintion address: %s\n", destName);
    printf("    destination port: %d\n\n", destPortNum);
}


void * runServer(void * arg) {
    printf("Server is running ...\n");
    u_int sinRemoteLen = sizeof(clientTx.sendToAddr);
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

void * runClient() {
    printf("Client is running ...\n");
    u_int sinRemoteLen = sizeof(clientTx.sendToAddr);
    char msg[MSG_MAX_LENGTH];
    int bytesTx = 0;

    char str2[INET_ADDRSTRLEN]= "";;
    while(1){
        inet_ntop(AF_INET, (struct sockaddr*) &clientTx.sendToAddr, str2, INET_ADDRSTRLEN);
        printf("destination socket address: %s\n", str2);
        printf("destination socket address size: %d\n", sinRemoteLen);

        scanf("%[^\n]s", msg);
        printf("message of size (%ld) is: %s\n", strlen(msg), msg);
        
        bytesTx = sendto(clientTx.socket, &msg, strlen(msg), 0, 
                (struct sockaddr*) &clientTx.sendToAddr, sinRemoteLen);
        if (bytesTx < 0) { 
            printf("Error in sending message!\n");
            terminateProgram(serverRx, clientTx);
        }
    }
    return NULL;
}




// Using this function to test to see if basic s-talk works
void setupAndReceiveMessage() {
    // // Setup my socket(will be server) to receive msg from peer
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

    while(1)
    {
        struct sockaddr_in * sinRemote = (struct sockaddr_in*)destInfoResults->ai_addr;
        unsigned int sinRemote_len = sizeof(struct sockaddr_in);
    
        char str1[INET_ADDRSTRLEN];
        char str2[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sin.sin_addr), str1, INET_ADDRSTRLEN);
        inet_ntop(AF_INET, &(sinRemote->sin_addr), str2, INET_ADDRSTRLEN);
        printf("our socket address: %s\n", str1);
        printf("our socket port: %d\n", ntohs(sin.sin_port));
        printf("destination socket address: %s\n", str2);
        printf("destination socket port: %d\n", ntohs(sinRemote->sin_port));


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
    }
    close(mySocketDescriptor);
    freeaddrinfo(destInfoResults);
}   

void setupAndReceiveMessage_DELETE_LATER() {
    // Setup my socket(will be server) to receive msg from peer
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin)); 
    sin.sin_family = AF_INET;
    sin.sin_port = htons(myPortNum); 
    sin.sin_addr.s_addr = htonl(INADDR_ANY); // TODO: change this to the IP address of our computer

    int mySocketDescriptor = socket(PF_INET, SOCK_DGRAM, PROTOCOL_DEFAULT);
    bind(mySocketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    // // Setup for getaddrinfo()
    // struct addrinfo hints;
    // memset(&hints, 0, sizeof(hints));
    // hints.ai_family = AF_INET;
    // hints.ai_socktype = SOCK_DGRAM;
    
    while(1){
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
        
        struct sockaddr_in sinRemote;
        unsigned int sinRemote_len = sizeof(sinRemote);
        
        // Receive message (like a server)
        char messageRx[MSG_MAX_LENGTH];
        // Blocking call to wait for msg to be received
        int bytesRx = recvfrom(mySocketDescriptor, messageRx, MSG_MAX_LENGTH, 0,
                        (struct sockaddr*) &sinRemote, &sinRemote_len); 
        // if (bytesRx == -1) { 
        //     printf("Error in receiving message! Exiting program");
        //     close(mySocketDescriptor);
        //     freeaddrinfo(destInfoResults);
        //     exit(0); 
        // }
        // Ensure it is a null terminated string
        int terminateMessageIndex = (bytesRx < MSG_MAX_LENGTH) ? bytesRx : MSG_MAX_LENGTH - 1;
        messageRx[terminateMessageIndex] = 0;
        // Print it out to console
        printf("Message received(%d bytes): '%s'\n", bytesRx, messageRx);

        // Send a message back (Reply)
        char replyTx[MSG_MAX_LENGTH] = "Your message was received!\n";
        // sprintf(replyTx, "Your message was received: \n(%s)\n", messageRx);
        // int replyResult = replyToSender(replyTx, mySocketDescriptor, sinRemote);
        replyToSender(replyTx, mySocketDescriptor, &sinRemote);
    }  
    close(mySocketDescriptor);
    // freeaddrinfo(destInfoResults);
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

