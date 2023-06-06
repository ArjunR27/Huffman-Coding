CC = gcc
CFLAGS = -Wall -Werror -ansi -pedantic

all: hencode hdecode

hencode: huffman.o ordered_list.o hencode.o
        $(CC) $(CFLAGS) -o hencode huffman.o ordered_list.o hencode.o

hdecode: huffman.o ordered_list.o hdecode.o
        $(CC) $(CFLAGS) -o hdecode huffman.o ordered_list.o hdecode.o

hdecode.o: hdecode.c
        $(CC) $(CFLAGS) -c -o hdecode.o hdecode.c

hencode.o: hencode.c
        $(CC) $(CFLAGS) -c -o hencode.o hencode.c

huffman.o: huffman.c
        $(CC) $(CFLAGS) -c -o huffman.o huffman.c

ordered_list.o: ordered_list.c
        $(CC) $(CFLAGS) -c -o ordered_list.o ordered_list.c
clean:
        rm -f *.o
