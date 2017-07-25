// ADT for a FIFO queue
// COMP1521 17s2 Week01 Lab Exercise
// Written by John Shepherd, July 2017
// Modified by Andrew Walls, z5165400

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Queue.h"

typedef struct QueueNode {
    int jobid;  // unique job ID
    int size;   // size/duration of job
    struct QueueNode *next;
} QueueNode;

struct QueueRep {
    int nitems;      // # of nodes
    QueueNode *head; // first node
    QueueNode *tail; // last node
};

// create a new node for a Queue
static
QueueNode *makeQueueNode(int id, int size)
{
    QueueNode *new;
    new = malloc(sizeof(struct QueueNode));
    assert(new != NULL);
    new->jobid = id;
    new->size = size;
    new->next = NULL;
    return new;
}

// free a queue node
// While this is only a single-line function, if the QueueNode type is later
// expanded to include more items, particularly alloc'd pointers, this function can
// handle deallocation of the items, rather than users of the Queue doing it themselves.
// Marked static as it's not defined in Queue.h.
static
void freeQueueNode(QueueNode *node)
{
    free(node);
}

// make a new empty Queue
Queue makeQueue()
{
    Queue new;
    new = malloc(sizeof(struct QueueRep));
    assert(new != NULL);
    new->nitems = 0; new->head = new->tail = NULL;
    return new;
}

// Recursive queue free
static
void freeQueueNodes(QueueNode *node) {
    // Terminator - the passed node pointer is null
    if(node == NULL) return;

    // Free the nodes after this node
    freeQueueNodes(node->next);
    // Then free this node
    freeQueueNode(node);
}

// release space used by Queue
void freeQueue(Queue q)
{
    assert(q != NULL);

    // Free the nodes contained in the queue
    freeQueueNodes(q->head);
    // Free the queue itself
    free(q);
}

// add a new item to tail of Queue
void enterQueue(Queue q, int id, int size)
{
    assert(q != NULL);

    // Create the new node
    QueueNode *node = makeQueueNode(id, size);
    // Ensure memory has been allocated
    assert(node != NULL);
    // Assign the head pointer if necessary
    if(!q->head) {
        q->head = node;
    }
    // Add the new node to the end of the queue if necessary
    if(q->tail) {
        q->tail->next = node;
    }
    // Reassign the tail pointer
    q->tail = node;
    // Increment nitems
    q->nitems += 1;
}

// remove item on head of Queue
// will return -1 if no node at head
int leaveQueue(Queue q)
{
    assert(q != NULL);
    // Ensure queue is not empty
    // ...Following the style of the rest of this file, this should _maybe_ be an assert?
    // Which would technically handle the edge case, but it's generally considered impolite
    // for a library to crash a program
    if(q->head == NULL) {
        return -1;
    }

    // Store the old head and id
    QueueNode *oldhead = q->head;
    int oldid = q->head->jobid;

    // Reassign the head of the queue
    q->head = q->head->next;

    // Clear the tail pointer if necessary
    if(oldhead == q->tail) {
        q->tail = NULL;
    }

    // Free the old head, return the id, and decrement nitems
    freeQueueNode(oldhead);
    q->nitems -= 1;
    return oldid;
}

// count # items in Queue
int lengthQueue(Queue q)
{
    assert(q != NULL);
    return q->nitems;
}

// Recursive approach to finding node volume
static
int volumeQueueNode(QueueNode *node)
{
    // Terminator - return initial volume of 0 when node if empty
    if(node == NULL) return 0;

    // Return the current node's size + the volume of the rest of the queue
    return node->size + volumeQueueNode(node->next);
}

// return total size in all Queue items
int volumeQueue(Queue q)
{
    assert(q != NULL);

    // Pass this down to the recursive function
    return volumeQueueNode(q->head);
}

// return size/duration of first job in Queue
// returns -1 if there is no 'first job'
int nextDurationQueue(Queue q)
{
    assert(q != NULL);

    // Ensure there is a head
    if(q->head == NULL) return -1;

    // Return the size of the head
    return q->head->size;
}


// display jobid's in Queue
void showQueue(Queue q)
{
    QueueNode *curr;
    curr = q->head;
    while (curr != NULL) {
        printf(" (%d,%d)", curr->jobid, curr->size);
        curr = curr->next;
    }
}
