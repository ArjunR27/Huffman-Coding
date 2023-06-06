#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ordered_list.h"

/*Initializes and allocates the linked list structure*/
LinkedList *createList()
{
    LinkedList* list = (LinkedList *)malloc(sizeof(LinkedList));
    if(list == NULL)
    {
        perror("malloc");
        return NULL;
    }
    list -> head = NULL;
    list -> size = 0;
    return list;
}

/* Adds a huffman node into the linked list, in ascending frequency
 *  * In the result of a frequency tie, it is checked and added in terms
 *   * of ASCII index value */
void add(LinkedList *l, HuffmanNode *hnode)
{

    Node* current;
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->item = hnode;
    newNode->next = NULL;

    if (l->head == NULL)
    {
        l->head = newNode;
        l->size++;
        return;
    }

    if (hnode->freq < l->head->item->freq ||
        (hnode->freq == l->head->item->freq && hnode->c < l->head->item->c))
    {
        newNode->next = l->head;
        l->head = newNode;
        l->size++;
        return;
    }

    current = l->head;
    while (current->next != NULL &&
           (current->next->item->freq < hnode->freq ||
            (current->next->item->freq == hnode->freq &&
             current->next->item->c < hnode->c)))
    {
        current = current -> next;
    }

    newNode -> next = current -> next;
    current -> next = newNode;
    l->size++;
}

/* Removes the head of the linkedlist
 *  * Sets the head to the next element */
Node *pop(LinkedList *l)
{
    Node *removedNode;
    if (l->head == NULL) {
        return NULL;
    }

    removedNode = l->head;
    l->head = l->head->next;
    l->size--;

    return removedNode;
}
