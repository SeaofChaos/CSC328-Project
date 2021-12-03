all: client server

library.o: Library.c
	gcc -g -O -c Library.c

client: Client.c Library.o
	gcc Client.c -o client

server: Server.c Library.o
	gcc Server.c -o server
