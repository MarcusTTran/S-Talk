# S-Talk

Simple messaging program that uses C to send and receive messages between two different computers given an IP address and port number to connect to. <br>

* Uses a connectionless UDP Transport Protocol, therefore messages are not 100% reliable
* Messages are sent and appear through each user's console in a chat-room-like manner
* Uses pthreads library to allow for messages to be sent and received at the same time

## HOW TO USE S-TALK:
To start an S-Talk instance with another socket, type: ___./s-talk \<MY PORT NUMBER\> \<DEST. IP ADDRESS\> \<DEST. PORT NUMBER\>___

To end your chat, type ___"!"___ into the terminal. If one peer does not close their connection, the chat stays open and can be rejoined. However, messages prior to joining the instance are not shown.

### CURRENT BUGS TO FIX!
* Sometimes, after the first message is sent to the peer, subsequent messages are not received 
