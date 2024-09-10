CC = arm-linux-gnueabihf-gcc
INCLUDE_DIR=$(PWD)
CFLAGS =-I$(INCLUDE_DIR) -Wall

all: dense_mm

clean:
	rm dense_mm

dense_mm: dense_mm.c
	$(CC) $(CFLAGS) dense_mm.c -o dense_mm

arr_search: arr_search.c
	$(CC) $(CFLAGS) arr_search.c -o arr_search -lm

