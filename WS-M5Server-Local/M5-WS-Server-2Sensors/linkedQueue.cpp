#include "linkedQueue.h"
#include <M5StickC.h> // for the Serial

// **********************************************
// LinkedList to store 2 distances as float
// **********************************************

// ***** Create Queue *****
struct LinkedQueue *createQueue() {
  // Malloc for the queue struct
  struct LinkedQueue *queue = (LinkedQueue*) malloc(sizeof(struct LinkedQueue));
  if (queue == nullptr) Serial.println("Malloc failed...");
  // Set the maxSize and size of the Queue
  queue->size = 0;
  // Set the first and last to null (queue is empty)
  queue->first = queue->last = NULL;
  // Return the pointer to the new queue
  return queue;
}

// ***** Enqueue *****
void enqueueDistances(struct LinkedQueue *queue, float d1, float d2) {
  // Malloc for a new node to enqueue
  struct Node *nNode = (Node*) malloc(sizeof(struct Node));
  if (nNode == nullptr) Serial.println("Malloc failed...");
  // New Node data and next
  nNode->sensor1 = d1;
  nNode->sensor2 = d2;
  nNode->next = nullptr;
  // If the queue is empty
  if (queue->size == 0) {
    // Insert new Node at first and at last 
    queue->first = queue->last = nNode;
  } else {
    // Enqueue new Node at last
    queue->last->next = nNode;
    // Mode last pointer
    queue->last = nNode;
  }
  // Update size
  queue->size++;
}

// ***** Free queue *****
void freeQueue(struct LinkedQueue *queue) {
  struct Node *currentNode = queue->first;
  // Free all nodes
  while (currentNode != nullptr) {
    struct Node *temp = currentNode;
    currentNode = currentNode->next;
    free(temp);
  }
  // Free the queue
  free(queue);
}

// ***** Iteration *****
void iterateQueue(struct LinkedQueue *queue) {
  struct Node *iterator = queue->first;
  while (iterator != nullptr) {
    float d1 = iterator->sensor1;
    float d2 = iterator->sensor2;
    Serial.println("Distance 1 : " + String(d1) + " \t|\t Distance 2 : " + String(d2));
    iterator = iterator->next;
  }
}
