all : s-talk

s-talk: client.o server.o list.o input.o output.o main.o
	gcc -g -pedantic -Wall -Werror -std=c99 -o s-talk -pthread main.o list.o client.o server.o

main.o: main.c 
	gcc -g -pedantic -Wall -Werror -std=c99 -c main.c

client.o: Client.c
	gcc -g -pedantic -Wall -Werror -std=c99 -c Client.c -o client.o

server.o: Server.c
	gcc -g -pedantic -Wall -Werror -std=c99 -c Server.c -o server.o

list.o: list.c
	gcc -g -pedantic -Wall -Werror -std=c99 -c list.c

input.o: input.c
	gcc -g -pedantic -Wall -Werror -std=c99 -c input.c

output.o: output.c
	gcc -g -pedantic -Wall -Werror -std=c99 -c output.c

clean:
	rm list.o server.o client.o input.o output.o main.o s-talk
# -lpthread (maybe)
