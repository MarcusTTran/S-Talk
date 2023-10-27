#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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
void * runClient(void * arg);
void * printIncomingMsg(void * arg);
void * getUserMessages(void * arg);
int replyToSender();
void freeItem(void * pItem);
void terminateProgram(struct Server server, struct Client client, List * pList1, List * pList2);


// Will hold the args from command line
int myPortNum, destPortNum;
char destName[REMOTE_NAME_BUFFER];

// GLOBAL Variables
List * pListRx;
List * pListTx;
struct Server serverRx;
struct Client clientTx;
// pthread_cond_t addOkToListRxCondVar = PTHREAD_COND_INITIALIZER;  Not sure if needed
static pthread_mutex_t modifyListRxMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t removeOkToListTxCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t modifyListTxMutex = PTHREAD_MUTEX_INITIALIZER;


int main(int argc, char *argv[]) {
    // Initialize data for program
    initTalkArgs(argc, argv);
    serverRx = server_constructor(AF_INET, SOCK_DGRAM, PROTOCOL_DEFAULT, myPortNum);
    clientTx = client_constructor(AF_INET, SOCK_DGRAM, myPortNum, destName, destPortNum);
    pListRx = List_create(); // for incoming messages and output thread
    pListTx = List_create(); // for outgoing messages and input thread
    
    // Create and run threads
    pthread_t server_thread;
    assert(pthread_create(&server_thread, NULL, runServer, &pListRx) == 0);
    pthread_t print_thread;
    assert(pthread_create(&print_thread, NULL, printIncomingMsg, &pListTx) == 0);


    // Cleanup threads and lists and structs
    pthread_join(&server_thread, NULL);
    pthread_join(&print_thread, NULL);
    // TODO: join the next two threads

    // Clean up mutexes and conditionals
    pthread_mutex_destroy(&modifyListRxMutex);
    pthread_mutex_destroy(&modifyListTxMutex);
    pthread_cond_destroy(&removeOkToListTxCondVar);
    


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

// thread which receives messages from 
void * runServer(void * pListAsVoid) {
    List * pList = (List*)pListAsVoid;
    socklen_t sinRemoteLen = sizeof(clientTx.sendToAddr);
    char messageRx[MSG_MAX_LENGTH];
    int bytesRx = 0;
    int terminateMessageIndex = 0;
    // Wait and receive message from remote connection through user specified port
    while(1) {
         bytesRx = recvfrom(serverRx.socket, messageRx, MSG_MAX_LENGTH, FLAGS_DEFAULT, 
                    (struct sockaddr*) &clientTx.sendToAddr, &sinRemoteLen);
        if (bytesRx == -1) { 
            printf("Error in receiving message!\n");
            terminateProgram(serverRx, clientTx, pListRx, pListTx);
        }
        // ensure string is null terminated
        terminateMessageIndex = (bytesRx < MSG_MAX_LENGTH) ? bytesRx : MSG_MAX_LENGTH - 1;
        messageRx[terminateMessageIndex] = 0;
        // Print it out to console
        printf("Message received(%d bytes): '%s'\n", bytesRx, messageRx); // TODO: delete later
        // add it to a shared list IF there is space on it. Don't forget to free lists at the end
        char * newMessage = (char*)malloc(strlen(messageRx) + 1);
        strcpy(newMessage, messageRx);
        void * newMessageVoid = (void*)newMessage;
        
        // TODO: Add maybe cond here (P call)
        pthread_mutex_lock(&modifyListRxMutex);
        if (List_append(pList, newMessageVoid) == -1) {
            printf("Error: List did not have enough nodes!\n");
        }
        // TODO: Add maybe cond here (V call)
        pthread_mutex_unlock(&modifyListRxMutex);
    }
    
    return NULL;
}

// print messages to screen that was given by server thread
void * printIncomingMsg(void * arg) {

    return NULL;
}

// Retrieves user's messages from consol until they enter "!"
void * getUserMessages(void * arg) {
    bool userDone = false;
    char * newMessage;
    while (!userDone) {
        newMessage = userInputMsg();
        if (strcmp(newMessage, END_TALK_STR) == 0) { // if user enters '!' then end program
            userDone = true;
            break;
        }
        void * newMessageAsVoid = (void*)newMessage;

        pthread_mutex_lock(&modifyListTxMutex);
        List_last(pListTx);
        List_append(pListTx, newMessageAsVoid);
        pthread_mutex_unlock(&modifyListTxMutex);

        // signal call for runCLient to remove item from list
        pthread_cond_signal(&removeOkToListTxCondVar);
    }
}

// Thread which gets messages from user 
void * runClient(void * arg) {
    char* messageTx;

    // pthread_mutex_lock(&modifyListTxMutex);
    while(1) { 
        pthread_mutex_lock(&modifyListTxMutex);
        while(List_count(pListTx) < 1) { // TODO: possibly revise this? Not sure yet
            pthread_cond_wait(&removeOkToListTxCondVar, &modifyListTxMutex);
        }
        
        if (List_first(pListTx) != NULL) {
            messageTx = List_remove(pListTx);
        }
        List_first(pListRx); // set list back to first item

        
        // DO MORE STUFF WITH THE NEW MESSAGE


        pthread_mutex_unlock(&modifyListTxMutex);
    }   
    // thread_mutex_unlock(&modifyListTxMutex);

    return NULL;
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

// FREE_FN for List implementation
void freeItem(void * pItem) {
    free(pItem);
    pItem = NULL;
}

// In the case of a major bug or error, call this to close free structs and 
// terminate the program. Additionally, can call this at the end to free structs
void terminateProgram(struct Server server, struct Client client, List * pList1, List * pList2) {
    closeClient(client);
    closeServer(server);
    List_free(pList1, freeItem);
    List_free(pList2, freeItem);
    exit(EXIT_FAILURE);
}


