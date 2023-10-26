// Will be used to handle the input from the keyboard to sending messages
// - Will have its own thread to execute its code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "headers.h"

// Returns a pointer to 
char * userInputMsg() {
    char msgRequest[MSG_MAX_LENGTH];
    memset(msgRequest, 0, MSG_MAX_LENGTH);

    // while (1) {
        fgets(msgRequest, sizeof(msgRequest), stdin);

    // }
    char * msgPtr = (char*)&msgRequest;
    return msgPtr;
}

