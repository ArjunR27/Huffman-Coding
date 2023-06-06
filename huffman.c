#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ordered_list.h"
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#define SIZE_ASCII 256
#define BUFFER_SIZE 4096


/* Writes the function strdup, to duplicate a string*/
char *strdup(const char *str)
{
    char *dup;
    size_t len = strlen(str) + 1;
    if(!(dup = malloc(len)))
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if (dup != NULL) {
        memcpy(dup, str, len);
    }
    return dup;
}

/*Reads the characters from the input file, and creates the frequency table*/
int *readFromFile(int fd)
{
    int *freq_list;
    ssize_t bytesRead, i;
    unsigned char buffer[BUFFER_SIZE];
    if(!(freq_list = (int *)calloc(SIZE_ASCII, sizeof(int))))
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    while((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    {
        for(i = 0; i < bytesRead; i++)
        {
            freq_list[buffer[i]] += 1;
        }
    }

   if(bytesRead == -1)
   {
        perror("read");
        exit(EXIT_FAILURE);
   }
   return freq_list;
}

/*Creates the linked list, by iterating through frequency table
 *   Then creates the huffman tree using the huffman algorithmi
 *     Returns the head node of the huffman tree */
HuffmanNode *createHuffTree(LinkedList *l, int* freqlist)
{
    int i;
    HuffmanNode *new = NULL;
    Node *node1 = NULL;
    Node *node2 = NULL;
    HuffmanNode *parent = NULL;
    for(i = 0; i < SIZE_ASCII; i++)
    {
        if(freqlist[i] != 0)
        {
            if(!(new = (HuffmanNode *)malloc(sizeof(HuffmanNode))))
            {
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            new -> c = i;
            new -> freq = freqlist[i];
            new -> left = NULL;
            new -> right = NULL;
            add(l, new);
        }
    }

    while (l->size > 1)
    {
        node1 = pop(l);
        node2 = pop(l);

        if(!(parent = (HuffmanNode *)malloc(sizeof(HuffmanNode))))
        {
             perror("malloc");
             exit(EXIT_FAILURE);
        }


        /*Intermediate Node; might need to set to NULL rather than have\
 *  *         the minimum character be set as the parent. */
        parent -> c = -1;

        parent -> freq = node1 -> item -> freq + node2 -> item -> freq;
        parent -> left = node1 -> item;
        parent -> right = node2 -> item;
        add(l, parent);
        free(node1);
        free(node2);
    }

    if(l -> head != NULL)
    {
        return l->head->item;
    }
    return NULL;
}

/* Recursive helper function, which traverses through the huffman tree
 *  * Creates the string code for each leaf of the tree. */
void createCodeHelper(HuffmanNode *node, char **codeTable, char *code)
{
    char *leftCode;
    char *rightCode;
    if(node -> left == NULL && node -> right == NULL)
    {
        codeTable[node -> c] = strdup(code);
        return;
    }
    if(!(leftCode = malloc((strlen(code) + 2) * sizeof(char))))
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(leftCode, code);
    strcat(leftCode, "0");
    createCodeHelper(node -> left, codeTable, leftCode);
    free(leftCode);

    if(!(rightCode = malloc((strlen(code) + 2) * sizeof(char))))
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    strcpy(rightCode, code);
    strcat(rightCode, "1");
    createCodeHelper(node -> right, codeTable, rightCode);
    free(rightCode);
}

/* Calls the recursive helper function*/
void createCode(HuffmanNode *node, char **codeTable)
{
    createCodeHelper(node, codeTable, "");
}

/* Creates the table of codes, storing them at the ASCII index
 *  * of the specific character. */
char **createCodeArray(HuffmanNode *root)
{
    char **codeTable;
    if(!(codeTable = (char **)calloc(SIZE_ASCII, sizeof(char *))))
    {
        perror("calloc");
        exit(EXIT_FAILURE);
    }
    createCode(root, codeTable);
    return codeTable;
}

/* Iterates through the codeTable and prints out the codes
 *  * for those ASCII values which are not NULL */
void printCodeTable(char **codeTable)
{
    int i;
    for (i = 0; i < SIZE_ASCII; i++)
    {
        if (codeTable[i] != NULL)
        {
            printf("CHARACTER %c CODE %s\n", (char)i, codeTable[i]);
            /*printf("0x%02x: %s\n", i, codeTable[i]);*/
        }
    }
}

/* Frees the memory allocation of the tree */
void freeEverythingHuffman(HuffmanNode *node)
{
    if (node != NULL)
    {
        freeEverythingHuffman(node -> left);
        freeEverythingHuffman(node -> right);
        free(node);
    }
    return;
}

/* Frees the code table */
void freeCodeTable(char **codeTable)
{
    int i;
    for(i = 0; i < SIZE_ASCII; i++)
    {
        free(codeTable[i]);
    }
    free(codeTable);
}
