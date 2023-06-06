#include "ordered_list.h"
#include "huffman.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#define SIZE_ASCII 256
#define BUFFER_SIZE 4096
#define ONE_BYTE 1
#define FOUR_BYTE 4
#define EIGHT_BITS 8
#define BIT_INDEX 7
#define SHIFT3 24
#define SHIFT2 16
#define SHIFT1 8


/*This function parses through the header
 * and creates the frequency list*/
int *parse_header(int fd)
{
    uint8_t count;
    int x;
    uint8_t character;
    uint32_t freq;
    uint8_t freqBytes[FOUR_BYTE];
    int i;
    int n;
    unsigned char buf[BUFFER_SIZE];
    int *freq_list;
    if(!(freq_list = (int *)calloc(SIZE_ASCII, sizeof(int))))
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    if(read(fd, &count, ONE_BYTE) != ONE_BYTE)
    {
        perror("read count");
        exit(EXIT_FAILURE);
    }
    if(count+1 == 0)
    {
        x = SIZE_ASCII;
    }
    else
    {
        x = count;
    }
    x++;

    for(i = 0; i < x; i++)
    {
        n = read(fd, buf, ONE_BYTE + FOUR_BYTE);
        if(n != ONE_BYTE + FOUR_BYTE)
        {
            perror("read character and frequency");
            exit(EXIT_FAILURE);
        }

        character = (uint8_t) buf[0];

        freqBytes[0] = (uint8_t)buf[1];
        freqBytes[1] = (uint8_t)buf[2];
        freqBytes[2] = (uint8_t)buf[3];
        freqBytes[3] = (uint8_t)buf[4];

        freq = (freqBytes[0] << SHIFT3) |
               (freqBytes[1] << SHIFT2) |
               (freqBytes[2] << SHIFT1) |
                freqBytes[3];
        if(freq == 0)
        {
            continue;
        }
        freq_list[character] = freq;
    }
    return freq_list;
}
/*This function decodes the input and writes to the output
 * It does this through using chunks of 4096*/

void decode(int input_fd, int output_fd, HuffmanNode *head, int *freq_list)
{
    unsigned char buffer[BUFFER_SIZE];
    int n, i, j;
    unsigned char byte = 0;
    HuffmanNode *node = head;
    uint64_t total_chars = 0;
    uint64_t chars_written = 0;
    uint8_t character;

    for(i = 0; i < SIZE_ASCII; i++)
    {
        total_chars += freq_list[i];
    }
    if(head != NULL)
    {
        character = (uint8_t) head -> c;
    }
    if(total_chars == freq_list[character])
    {
        for(i = 0; i < freq_list[character]; i++)
        {
            write(output_fd, &character, ONE_BYTE);
        }
        return;
    }
    while(chars_written < total_chars)
    {
        n = read(input_fd, buffer, BUFFER_SIZE);
        if(n == -1)
        {
            perror("read error");
            exit(EXIT_FAILURE);
        }
        else if(n == 0)
        {
            break;
        }

        for(i = 0; i < n; i++)
        {
            byte = buffer[i];
            for(j = 0; j < EIGHT_BITS; j++)
            {
                if((byte >> (BIT_INDEX - j)) & 1)
                {
                    node = node -> right;
                }
                else
                {
                    node = node -> left;
                }
                if(node == NULL)
                {
                    node = head;
                    break;
                }
                if(node -> left == NULL && node -> right == NULL)
                {
                    character = (uint8_t) (node -> c);
                    write(output_fd, &character, ONE_BYTE);
                    chars_written++;
                    node = head;
                }

                if(chars_written == total_chars)
                {
                    break;
                }
            }

        }
    }
}



/*Control the input and output of the
 * huffman decoding functionality*/
int main(int argc, char *argv[])
{
    int input_fd;
    int output_fd;
    int *freq_list;
    LinkedList *l;
    HuffmanNode *head;
    struct stat st;

    input_fd = STDIN_FILENO;
    output_fd = STDOUT_FILENO;
    if(argc > 1 && strcmp(argv[1], "-") != 0)
    {
        input_fd = open(argv[1], O_RDONLY);
        if(input_fd == -1)
        {
            perror("OPEN INPUT FILE");
            exit(EXIT_FAILURE);
        }
    }


    if(argc > 2 && strcmp(argv[2], "-") != 0)
    {
        output_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(output_fd == -1)
        {
            perror("OPEN OUTPUT FILE");
            exit(EXIT_FAILURE);
        }
    }


    fstat(input_fd, &st);
    if(st.st_size == 0)
    {
        close(input_fd);
        close(output_fd);
        return 0;
    }

    l = createList();
    freq_list = parse_header(input_fd);
    head = createHuffTree(l, freq_list);

    decode(input_fd, output_fd, head, freq_list);

    free(freq_list);
    freeEverythingHuffman(head);
    free(l -> head);
    free(l);
    close(input_fd);
    close(output_fd);
    return 0;
}
