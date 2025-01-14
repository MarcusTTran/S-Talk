
// Will be used to handle the input from the keyboard to sending messages
// - Will have its own thread to execute its code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"
#include <pthread.h>
#include "list.h"
#include "input.h"

// TODO: maybe create a tag for user when they send messages? Like "You: "? TBD

// Dynamically allocates a string from user input and returns a pointer to it
// - String will be null terminated
// - Will not have an endline at the end 
char * userInputMsg() {
    char msgRequest[MSG_MAX_LENGTH];
    memset(msgRequest, 0, MSG_MAX_LENGTH);
    fgets(msgRequest, sizeof(msgRequest), stdin);
    // null-terminate the string
    size_t msgLen = strlen(msgRequest);
    if (msgLen > 0 && msgRequest[msgLen - 1] == '\n') {
        msgRequest[msgLen - 1] = '\0'; 
    }
    // send it as a pointer
    char * msgPtr = (char*)malloc(strlen(msgRequest) + 1);
    strncpy(msgPtr, msgRequest, strlen(msgRequest));
    return msgPtr;
}


