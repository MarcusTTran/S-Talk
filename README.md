# S-Talk

Simple peer-to-peer messaging program that uses C to send and receive messages between two different computers given an IP address and port number to connect to. <br>

* Uses a connectionless UDP Transport Protocol, therefore messages are not 100% reliable
* Messages are sent and appear through each user's console in a chat-room-like manner
* Uses pthreads library to allow for messages to be sent and received asynchronously by each peer

## HOW TO USE S-TALK:
To start an S-Talk instance with another socket, type into the console: <br>
&emsp; *./s-talk \<MY PORT NUMBER\> \<DEST. IP ADDRESS\> \<DEST. PORT NUMBER\>* 

Once **both peers** with the downloaded files run an instance of S-Talk with each other's correct socket information, 
they can begin sending messages.

### CURRENT BUGS TO FIX!
* Sometimes, after the first message is sent to the peer, subsequent messages are not received 
