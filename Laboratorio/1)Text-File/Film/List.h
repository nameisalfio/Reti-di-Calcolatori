#ifndef LIST_H
#define LIST_H

typedef struct Node
{
    Film key;
    struct Node* next;
}Node;

void printNode(Node* node)
{   
    if(node)
        printFilm(node->key);
    else
        printf("NULLPTR");
}

typedef struct List
{
    Node* head;
}List;

bool isEmpty(List* list)
{
    return !(list->head);
}

void insertHead(List* list, Film val)
{
    Node* toinsert = (Node*)malloc(sizeof(Node));
    toinsert->key = val;
    toinsert->next = NULL;

    if(isEmpty(list))
    {
        list->head = toinsert;
        return;
    }
    toinsert->next = list->head;
    list->head = toinsert;
}

void insertTail(List* list, Film val)
{    
    if(isEmpty(list))
    {
        insertHead(list, val);
        return;
    }

    Node* toinsert = (Node*)malloc(sizeof(Node));
    toinsert->key = val;
    toinsert->next = NULL;

    Node* cur = list->head;
    while(cur->next)
        cur = cur->next;
    cur->next = toinsert;
}

void removeHead(List* list)
{
    if(isEmpty(list))
    {
        puts("\nEmpty List\n");
        return;
    }

    Node* toremove = list->head;
    list->head = list->head->next;
    free(toremove);
}

void removeTail(List* list)
{
    if(isEmpty(list))
    {
        puts("\nEmpty List\n");
        return;
    }

    Node* toremove = list->head;
    Node* prev = NULL;

    while(toremove->next)
    {
        prev = toremove;
        toremove = toremove->next;
    }
    prev->next = NULL;
    free(toremove);
}

Node* search(List* list, Film val)
{
    if(isEmpty(list))
    {
        puts("\nEmpty List\n");
        return NULL;
    }

    Node* cur = list->head;
    while(cur->next && !equal(cur->key, val))
        cur = cur->next;
    
    if(!equal(cur->key, val))
    {
        printf("\nElement not found\n");
        return NULL;
    }
    return cur;
}

void printList(List* list)
{
    if(isEmpty(list))
    {
        puts("\nEmpty List\n");
        return;
    }

    Node* cur = (Node*)malloc(sizeof(Node));
    cur = list->head;
    printf("Head\n\n");
    while(cur)
    {
        printNode(cur);
        cur = cur->next;
    }
    puts("\nNIL");
}

#endif