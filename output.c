
// Will be used to handle the output from the port to the screen
// - Will have its own thread to execute its code
#include <stdio.h>
#include <string.h>
#include "headers.h"
#include "output.h"

void printMessage(const char* msgReceived, const char* userName) {
    
    // printf("You: ");
    printf("Friend: ");
    fflush(stdout);

    size_t msgLen = strlen(msgReceived);
    write(STDOUT_FILENO, msgReceived, msgLen);
    
    printf("\n");
    fflush(stdout);
}

void printEndMessage() {
    char * endMessage = "\n\nClosing connection to peer...\n\n Ending program. Goodbye!\n";
    size_t endMsgLen = strlen(endMessage);
    write(STDOUT_FILENO, endMessage, endMsgLen);
}

