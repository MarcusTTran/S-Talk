// Will be used to handle the output from the port to the screen
// - Will have its own thread to execute its code
#include <stdio.h>
#include <string.h>
#include "headers.h"

void printMessage(const char* msgReceived, const char* userName) {
    printf("\n\t%s: %s\n", userName, msgReceived);
}