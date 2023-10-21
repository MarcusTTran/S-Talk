all : s-talk

s-talk: 
	gcc -g -pthread -pedantic -Wall -Werror -o -c talk.c list.o client.o server.o

client.o:
	gcc -g -pedantic -Wall -Werror -c Client.c -o client.o

server.o:
	gcc -g -pedantic -Wall -Werror -c Server.c -o server.o

list.o: list.c
	gcc -g -pedantic -Wall -Werror -c list.c

clean:

# -lpthread (maybe)
