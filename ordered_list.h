
/*Defines HuffmanNode structure*/
typedef struct HuffmanNode
{
    int c;
    int freq;
    struct HuffmanNode *left;
    struct HuffmanNode *right;
} HuffmanNode;

/* Defines Node structure */
typedef struct Node
{
    struct HuffmanNode *item;
    struct Node* next;

} Node;

/* Defines LinkedList structure */
typedef struct LinkedList
{
    struct Node *head;
    int size;
} LinkedList;


/* Defines the ordered_list.c functions so they can be used in huffman.c */
LinkedList *createList();
void add(LinkedList *l, HuffmanNode *hnode);
Node *pop(LinkedList *l);
void freeOrderedList(LinkedList *l);


