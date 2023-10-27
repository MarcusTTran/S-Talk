// Common header file to include other headers and define 
// constants used across many files

// Networking includes
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>


typedef unsigned long u_long;

#define PROTOCOL_DEFAULT 0
#define FLAGS_DEFAULT 0
#define MSG_MAX_LENGTH 256
#define END_TALK_STR "!"

