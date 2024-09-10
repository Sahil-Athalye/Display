CC = arm-linux-gnueabihf-gcc
INCLUDE_DIR=$(PWD)/../include
CFLAGS =-I$(INCLUDE_DIR) -Wall

all: arr_search

clean:
	rm arr_search

arr_search: arr_search.c
	$(CC) $(CFLAGS) arr_search.c -o arr_search
