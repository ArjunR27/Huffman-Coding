#include "ordered_list.h"
#include "huffman.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#define SIZE_ASCII 256
#define BUFFER_SIZE 4096
#define ONE_BYTE 1
#define FOUR_BYTE 4
#define EIGHT_BITS 8
#define BIT_INDEX 7
#define SHIFT3 24
#define SHIFT2 16
#define SHIFT1 8


/*Function that develops the header from the
 * frequency list*/
void writeHeader(int fd, int *freqlist)
{
    uint8_t count = 0;
    uint8_t character;
    uint32_t freq;
    uint8_t freqBytes[4];
    int i;
    for(i = 0; i < SIZE_ASCII; i++)
    {
        if(freqlist[i] != 0)
        {
            count++;
        }
    }
    count--;

    write(fd, &count, ONE_BYTE);

    for(i = 0; i < SIZE_ASCII; i++)
    {
        if(freqlist[i] != 0)
        {
            character = (uint8_t)i;
            freq = (uint32_t)freqlist[i];

            freqBytes[0] = (uint8_t)((freq >> SHIFT3) & 0xFF);
            freqBytes[1] = (uint8_t)((freq >> SHIFT2) & 0xFF);
            freqBytes[2] = (uint8_t)((freq >> SHIFT1) & 0xFF);
            freqBytes[3] = (uint8_t)(freq & 0xFF);
            write(fd, &character, ONE_BYTE);
            write(fd, freqBytes, FOUR_BYTE);
        }
    }
}


/* Actually encodes the file.
 * Iterates through the file in chunks of 4096 bytes
 * and writes out the codes*/
void encode(int infd, int outfd, int *freqlist, char **codes)
{
    unsigned char inbuf[BUFFER_SIZE];
    unsigned char outbuf[BUFFER_SIZE];
    int inbufsize;
    int outbufsize = 0;
    int remainingbits = 0;
    uint8_t currbyte = 0;
    int i, j;
    char *code;
    char bit;
    while ((inbufsize = read(infd, inbuf, BUFFER_SIZE)) > 0)
    {
        for(i = 0; i < inbufsize; i++)
        {
            code = codes[(int)inbuf[i]];
            for(j = 0; j < strlen(code); j++)
            {
                bit = code[j];
                if (bit == '1')
                {
                    currbyte |= (1 << (BIT_INDEX - remainingbits));
                }
                remainingbits++;

                if (remainingbits == EIGHT_BITS)
                {
                    outbuf[outbufsize++] = currbyte;
                    currbyte = 0;
                    remainingbits = 0;
                    if (outbufsize == BUFFER_SIZE)
                    {
                        write(outfd, outbuf, outbufsize);
                        outbufsize = 0;
                    }
                }
            }
        }
    }
    /*Extra checks were needed to make sure that all the bits
 *    were written to the file*/
    if (remainingbits > 0)
    {
        outbuf[outbufsize++] = currbyte;
    }

    if (outbufsize > 0)
    {
        write(outfd, outbuf, outbufsize);
    }
}

/*Executes the huffman encoding files, controls
 * the input and output through command line arguments
 * and stdin/stdout */
int main(int argc, char *argv[])
{
    int input_fd;
    int output_fd;
    int *freq_list;
    HuffmanNode *head;
    char **codeTable;
    LinkedList *l;


    input_fd = open(argv[1], O_RDONLY);
    if(input_fd == -1)
    {
        perror("FILE NOT FOUND");
        return 0;
    }


    if(argc == 3)
    {
        output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(output_fd == -1)
        {
            perror("FILE NOT FOUND");
            return 0;
        }
    }
    else
    {
        output_fd = STDOUT_FILENO;
    }


    l = createList();
    freq_list = readFromFile(input_fd);


    lseek(input_fd, 0, SEEK_SET);

    head = createHuffTree(l, freq_list);
    if(l -> size == 0)
    {
        free(freq_list);
        free(l);
        close(input_fd);
        close(output_fd);
        return 0;
    }

    codeTable = createCodeArray(head);
    writeHeader(output_fd, freq_list);
    encode(input_fd, output_fd, freq_list, codeTable);

    free(freq_list);
    freeCodeTable(codeTable);
    freeEverythingHuffman(head);
    free(l -> head);
    free(l);
    close(output_fd);
    close(input_fd);
    return 0;
}
