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
void * runServer(void * pListAsVoid);
void * runClient(void * pListAsVoid);
void * printIncomingMsg(void * pListAsVoid);
void * getUserMessages(void * pListAsVoid);
int replyToSender(const char message[], int mySocket, struct sockaddr_in * sinRemote);
void deallocateMutexesAndConditionals(pthread_mutex_t * mutex1, pthread_mutex_t * mutex2, pthread_cond_t * cond1, pthread_cond_t * cond2);
void freeItem(void * pItem);
void prepareToTerminateProgram(struct Server server, struct Client client, List * pList1, List * pList2);


// Will hold the args from command line
int myPortNum, destPortNum;
char destName[REMOTE_NAME_BUFFER];

// GLOBAL Variables
List * pListRx;
List * pListTx;
struct Server serverRx;
struct Client clientTx;
static pthread_cond_t removeOkToListRxCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t modifyListRxMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t removeOkToListTxCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t modifyListTxMutex = PTHREAD_MUTEX_INITIALIZER;

static pthread_cond_t endProgramCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t masterThreadMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t printOkCondVar = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t stdoutMutex = PTHREAD_MUTEX_INITIALIZER;
// bool userExit = false;


int main(int argc, char *argv[]) {
    // Initialize data for program
    initTalkArgs(argc, argv);
    serverRx = server_constructor(AF_INET, SOCK_DGRAM, PROTOCOL_DEFAULT, myPortNum);
    clientTx = client_constructor(AF_INET, SOCK_DGRAM, myPortNum, destName, destPortNum);
    pListRx = List_create(); // for incoming messages and output thread
    pListTx = List_create(); // for outgoing messages and input thread
    
    pthread_mutex_lock(&masterThreadMutex);
    // Create and run threads
    pthread_t server_thread;
    assert(pthread_create(&server_thread, NULL, runServer, &pListRx) == 0);
    pthread_t print_thread;
    assert(pthread_create(&print_thread, NULL, printIncomingMsg, &pListRx) == 0);
    pthread_t keyboard_thread;
    assert(pthread_create(&keyboard_thread, NULL, getUserMessages, &pListTx) == 0);
    pthread_t client_thread;
    assert(pthread_create(&client_thread, NULL, runClient, pListTx) == 0);

    // Wait until the user wants to exit    
    pthread_cond_wait(&endProgramCondVar, &masterThreadMutex);

    // Cancel threads remaining 3 threads
    pthread_cancel(server_thread);
    pthread_cancel(print_thread);
    pthread_cancel(client_thread);

    // Cleanup threads and lists and structs
    pthread_join(server_thread, NULL);
    pthread_join(print_thread, NULL);
    pthread_join(keyboard_thread, NULL);
    pthread_join(client_thread, NULL);

    pthread_mutex_unlock(&masterThreadMutex);
    // Clean up mutexes and conditionals
    deallocateMutexesAndConditionals(&modifyListRxMutex, &modifyListTxMutex, 
                                &removeOkToListRxCondVar, &removeOkToListTxCondVar);
    prepareToTerminateProgram(serverRx, clientTx, pListRx, pListTx);
    printEndMessage();

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
}

// thread which receives messages from peer and puts it into a shared list
// then signals for another thread to print it out to the console
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
            prepareToTerminateProgram(serverRx, clientTx, pListRx, pListTx);
            exit(EXIT_FAILURE);
        }
        // ensure string is null terminated
        terminateMessageIndex = (bytesRx < MSG_MAX_LENGTH) ? bytesRx : MSG_MAX_LENGTH - 1;
        messageRx[terminateMessageIndex] = 0;
        // // Print it out to console
        // printf("Message received(%d bytes): '%s'\n", bytesRx, messageRx); // TODO: delete later
        // add it to a shared list IF there is space on it. Don't forget to free lists at the end
        char * newMessage = (char*)malloc(strlen(messageRx) + 1);
        strcpy(newMessage, messageRx);
        void * newMessageVoid = (void*)newMessage;
        
        pthread_mutex_lock(&modifyListRxMutex);
        List_last(pList);
        if (List_append(pList, newMessageVoid) == -1) {
            printf("Error: List did not have enough nodes!\n");
            prepareToTerminateProgram(serverRx, clientTx, pListRx, pListTx);
            exit(EXIT_FAILURE);
        } else {
            pthread_cond_signal(&removeOkToListRxCondVar);
        }
        pthread_mutex_unlock(&modifyListRxMutex);
    }
    
    return NULL;
}

// print messages to screen that was given by server thread
void * printIncomingMsg(void * pListAsVoid) {
    List * pList = (List*)pListAsVoid;
    char* messageRx;
    while(1) {
        pthread_mutex_lock(&modifyListRxMutex);
        while (List_count(pList) < 1) {
            pthread_cond_wait(&removeOkToListRxCondVar, &modifyListRxMutex);
        }
        if (List_first(pList) != NULL) {
            messageRx = (char*)List_remove(pList);
        } else {
            printf("Error: there was no item on list to remove in printIncomingMsg()\n");
            prepareToTerminateProgram(serverRx, clientTx, pListRx, pListTx);
            exit(EXIT_FAILURE);
        }
        pthread_mutex_unlock(&modifyListRxMutex);
        
        pthread_mutex_lock(&stdoutMutex);
        printMessage(messageRx, PEER_NAME_STR);
        pthread_mutex_unlock(&stdoutMutex);
    }
   
    return NULL;
}

// Retrieves user's messages from consol until they enter "!"
void * getUserMessages(void * pListAsVoid) {
    List * pList = (List*)pListAsVoid;
    char * newMessage;
    while (1) {
        newMessage = userInputMsg();
        if (strcmp(newMessage, END_TALK_STR) == 0) { // if user enters '!' then end program
            // userExit = true;
            pthread_cond_signal(&endProgramCondVar);
            pthread_exit(NULL);
        }
        void * newMessageAsVoid = (void*)newMessage;

        pthread_mutex_lock(&modifyListTxMutex);
        List_last(pList);
        if (List_append(pList, newMessageAsVoid) == -1) {
            printf("Error: In getUserMessages - List did not have enough nodes!\n");
        //     prepareToTerminateProgram(serverRx, clientTx, pListRx, pListTx);
        //     exit(EXIT_FAILURE);
        // } else { 
            // signal call for runCLient to remove item from list
            pthread_cond_signal(&removeOkToListTxCondVar);
        }
        pthread_mutex_unlock(&modifyListTxMutex);

    }
    return NULL;
}

// Thread which gets messages from user 
void * runClient(void * pListAsVoid) {
    List * pList = (List*)pListAsVoid;
    char* messageTx;

    while(1) { 
        pthread_mutex_lock(&modifyListTxMutex);
        while(List_count(pList) < 1) { // TODO: possibly revise this? Not sure yet
            pthread_cond_wait(&removeOkToListTxCondVar, &modifyListTxMutex);
        }
        
        if (List_first(pList) != NULL) {
            messageTx = (char*)List_remove(pList);
        } else {
            printf("Error: there was no item on list to remove in runClient()\n");
            // prepareToTerminateProgram(serverRx, clientTx, pListRx, pListTx);
            // exit(EXIT_FAILURE);
        }
        List_first(pList); // set list back to first item
        
        int reply_result = replyToSender(messageTx, clientTx.socket, clientTx.sendToAddr);
        if(reply_result == -1) {
            printf("Error occured in sending message to peer!\n");
            prepareToTerminateProgram(serverRx, clientTx, pListRx, pListTx);
            exit(EXIT_FAILURE);
        }
        pthread_mutex_unlock(&modifyListTxMutex);
    }   

    return NULL;
}

// Sends a reply in the form of a char[] to a sender name sinRemote using a user-specified socket
int replyToSender(const char message[], int mySocket, struct sockaddr_in * sinRemote) { 
    socklen_t sinRemoteLen = sizeof(*sinRemote);
    int bytesTx = sendto(mySocket, message, strlen(message), FLAGS_DEFAULT, 
        (struct sockaddr*) &sinRemote, sinRemoteLen);
    if (bytesTx == -1) {
        return -1;
    } 
    return 0;
}

void deallocateMutexesAndConditionals(pthread_mutex_t * mutex1, pthread_mutex_t * mutex2, pthread_cond_t * cond1, pthread_cond_t * cond2) {
    // pthread_mutex_destroy(mutex1);
    // pthread_mutex_destroy(mutex2);
    // pthread_cond_destroy(cond1);
    // pthread_cond_destroy(cond2);
    
    // Don't need this code above since threads are static
}

// FREE_FN for List implementation
void freeItem(void * pItem) {
    free(pItem);
    pItem = NULL;
}

// In the case of a major bug or error, call this to close free structs and 
// terminate the program. Additionally, can call this at the end to free structs
void prepareToTerminateProgram(struct Server server, struct Client client, List * pList1, List * pList2) {
    closeClient(client);
    closeServer(server);
    List_free(pList1, freeItem);
    List_free(pList2, freeItem);
}


