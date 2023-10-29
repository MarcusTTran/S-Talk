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

struct sockaddr_in tempAddrOur;
struct sockaddr_in tempAddrDestination;

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

    clientTx = client_constructor(AF_INET, SOCK_DGRAM, myPortNum, destName, destPortNum);
    
    serverRx = server_constructor(AF_INET, SOCK_DGRAM, PROTOCOL_DEFAULT, myPortNum);
    
    printf("\nAfter server and client creation, data:\n");

    printf("    destination socket address: %s\n", inet_ntoa(clientTx.sendToAddr.sin_addr));
    printf("    destination socket port: %d\n", ntohs(clientTx.sendToAddr.sin_port));
    
    printf("    our socket address: %s\n", inet_ntoa(serverRx.address.sin_addr));
    printf("    our socket port: %d\n\n", ntohs(serverRx.address.sin_port));
    

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

    printf("\nAfter initialization of ports and addresses, data:\n");
    printf("    our port: %d\n", myPortNum);
    printf("    destintion address: %s\n", destName);
    printf("    destination port: %d\n\n", destPortNum);

    return;
}


void * runServer(void * arg) {
    printf("Server is running ...\n");
    u_int sinRemoteLen = sizeof(clientTx.sendToAddr);
    char messageRx[MSG_MAX_LENGTH];
    int bytesRx = 0;
    int bytesTX = 0;
    int count = 0;
    int terminateMessageIndex = 0;
    // Wait and receive message from remote connection through user specified port
    while(1) {
        // messageRx[MSG_MAX_LENGTH] = ' ';
        printf("count RECEIVE: %d\n", count);
        printf("    destination socket address: %s\n", inet_ntoa(clientTx.sendToAddr.sin_addr));
        printf("    destination socket port: %d\n", ntohs(clientTx.sendToAddr.sin_port));
        
        printf("    our socket address: %s\n", inet_ntoa(serverRx.address.sin_addr));
        printf("    our socket port: %d\n\n", ntohs(serverRx.address.sin_port));
    
        bytesRx = 0;
         bytesRx = recvfrom(serverRx.socket, messageRx, MSG_MAX_LENGTH, FLAGS_DEFAULT, 
                    (struct sockaddr*) &clientTx.sendToAddr, &sinRemoteLen);
        if (bytesRx == -1) { 
            printf("Error in receiving message!\n");
            // terminateProgram(serverRx, clientTx);
        }
        // ensure string is null terminated
        terminateMessageIndex = (bytesRx < MSG_MAX_LENGTH) ? bytesRx : MSG_MAX_LENGTH - 1;
        messageRx[terminateMessageIndex] = 0;
        // Print it out to console
        printf("Message received(%d bytes): '%s'\n", bytesRx, messageRx);
        // TODO: add it to a shared list
        char relpyMSG[MSG_MAX_LENGTH] = "message was received!\n";
        bytesTX = sendto(clientTx.socket, relpyMSG, strlen(relpyMSG), FLAGS_DEFAULT,
                (struct sockaddr*) &clientTx.sendToAddr, sizeof(clientTx.sendToAddr));
        if(bytesTX < 0)
        {
            printf("Error in sending the reply message!\n(sendto did not work back)\n");
        }
        count++;
    }
    
    return NULL;
}

void * runClient() {
    printf("Client is running ...\n");
    // u_int sinRemoteLen = sizeof(clientTx.sendToAddr);
    char msg[MSG_MAX_LENGTH] = "Hello world 1";
    int bytesTx = 0;
    bool check = 0;
    int count = 0;
    // char x;

    while(check == 0){
        printf("count SEND: %d\n", count);
        printf("    destination socket address: %s\n", inet_ntoa(clientTx.sendToAddr.sin_addr));
        printf("    destination socket port: %d\n", ntohs(clientTx.sendToAddr.sin_port));
        
        printf("    our socket address: %s\n", inet_ntoa(serverRx.address.sin_addr));
        printf("    our socket port: %d\n\n", ntohs(serverRx.address.sin_port));
    
        // printf("    destination socket address: %s\n", inet_ntoa(clientTx.sendToAddr.sin_addr));
        // printf("    destination socket address size: %d\n", sinRemoteLen);

        // scanf("%c", &x);
        fgets(msg,MSG_MAX_LENGTH,stdin);
        
        printf("message of size (%ld) is sent: %s\n", strlen(msg), msg);
        
        bytesTx = sendto(clientTx.socket, msg, strlen(msg) + 1, 0, 
                (struct sockaddr*) &clientTx.sendToAddr, sizeof(clientTx.sendToAddr));
        if (bytesTx < 0) { 
            printf("Error in sending message!\n");
            // terminateProgram(serverRx, clientTx);
        }
        if(strcmp(msg, "!") == 0)
        {
            check = 1;
        }
        count++;
    }
    return NULL;
}



// In the case of a major bug or error, call this to close free structs and 
// terminate the program
void terminateProgram(struct Server server, struct Client client) {
    closeClient(client);
    closeServer(server);
    exit(EXIT_FAILURE);
}

