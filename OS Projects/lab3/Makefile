CFLAGS =-g -Wall

all: server client

server: server.c tree
	gcc $(CFLAGS) -o server server.c tree.o

client: client.c tree
	gcc $(CFLAGS) -o client client.c tree.o

tree:
	gcc $(CFLAGS) -c tree.c