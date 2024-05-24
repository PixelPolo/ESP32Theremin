#ifndef LINKED_QUEUE_H
#define LINKED_QUEUE_H

// **********************************************
// HEADER FILE
// LinkedList to store 2 distances as float
// **********************************************

// ***** Node struct *****
struct Node
{
    float sensor1;
    float sensor2;
    struct Node *next;
};

// ***** Queue struct *****
struct LinkedQueue
{
    int size;
    struct Node *first;
    struct Node *last;
};

// ***** Create Queue *****
LinkedQueue* createQueue();

// ***** Enqueue Distances *****
void enqueueDistances(LinkedQueue *queue, float d1, float d2);

// ***** Free queue *****
void freeQueue(LinkedQueue *queue);

// ***** Iteration *****
void iterateQueue(LinkedQueue *queue);

#endif
