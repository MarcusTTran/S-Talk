all : s-talk

s-talk: client.o server.o list.o input.o output.o main.o
	gcc -g -pthread -pedantic -Wall -Werror -o s-talk main.o list.o client.o server.o

main.o: main.c 
	gcc -g -pthread -pedantic -Wall -Werror -c main.c

client.o: Client.c
	gcc -g -pedantic -Wall -Werror -c Client.c -o client.o

server.o: Server.c
	gcc -g -pedantic -Wall -Werror -c Server.c -o server.o

list.o: list.c
	gcc -g -pedantic -Wall -Werror -c list.c

input.o: input.c
	gcc -g -pedantic -Wall -Werror -c input.c

output.o: output.c
	gcc -g -pedantic -Wall -Werror -c output.c

clean:
	rm list.o server.o client.o input.o output.o main.o s-talk
# -lpthread (maybe)
