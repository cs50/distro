/*************************************************************************
 * queue.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Implements a simple queue structure for char* s.
 ************************************************************************/

// for strdup() in the testing code
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 10

typedef struct
{
    // the index of the first element in the queue
    int head;

    // storage for the elements in the queue
    char* strings[CAPACITY];

    // the size of the queue
    int size;
}
queue;

// global variable for this program's one queue
queue q;

/**
 * Puts a new element into the queue into the "end" of the data structure
 * so that it will be retrived after the other elements already in the
 * queue.
 */
bool enqueue(char* string)
{
    // if we're at capacity, we can't add anything more to our queue
    if (q.size == CAPACITY)
    {
        return false;
    }

    // calculate the index of the "last" slot in the queue
    int index = (q.head + q.size) % CAPACITY;

    // add the new element to our queue
    q.strings[index] = string;

    // adjust the size appropriately
    q.size++;

    return true;
}

/**
 * Retrieves ("dequeues") the first element in the queue, following the
 * the "first-in, first-out" (FIFO) ordering of the data structure.
 * Reduces the size of the queue and adjusts the head to the next element.
 */
char* dequeue()
{
    // if nothing's in our queue, we can't retrieve anything!
    if (q.size == 0)
    {
        return NULL;
    }

    // grab the last string in our stack
    char* last = q.strings[q.head];

    // adjust the head of our queue to point to the next element
    // wrap back to the beginning if necessary
    q.head = (q.head + 1) % CAPACITY;

    // adjust the size appropriately
    q.size--;

    return last;
}

/**
 * Implements some simple test code for our queue
 */
int main(void)
{
    printf("Enqueueing %d strings...", CAPACITY);
    for (int i = 0; i < CAPACITY; i++)
    {
        char str[12];
        sprintf(str, "%d", i);
        enqueue(strdup(str));
    }
    printf("done!\n");

    printf("Making sure that the queue size is indeed %d...", CAPACITY);
    assert(q.size == CAPACITY);
    printf("good!\n");

    printf("Making sure that enqueue() now returns false...");
    assert(!enqueue("too much!"));
    printf("good!\n");

    printf("Dequeueing everything...");
    char* str_array[CAPACITY];
    for (int i = 0; i < CAPACITY; i++)
    {
        str_array[i] = dequeue();
    }
    printf("done!\n");

    printf("Making sure that dequeue() returned values in FIFO order...");
    for (int i = 0; i < CAPACITY; i++)
    {
        char str[12];
        sprintf(str, "%d", i);
        assert(strcmp(str_array[i], str) == 0);
        free(str_array[i]);
    }
    printf("good!\n");

    printf("Making sure that the queue is now empty...");
    assert(q.size == 0);
    printf("good!\n");

    printf("Making sure that dequeue() now returns NULL...");
    assert(dequeue() == NULL);
    printf("good!\n");

    printf("Making sure that the head wraps around appropriately...");
    for (int i = 0; i < CAPACITY; i++)
    {
        assert(enqueue("cs50!"));
    }

    // to make sure that they adjust the head pointer appropriately, we'll
    // enqueue and dequeue a bunch of times to make sure they don't just
    // walk off the end of the char* strings[] array
    for (int i = 0; i < CAPACITY * 10; i++)
    {
        for (int j = 0; j < CAPACITY / 2; j++)
        {
            assert(dequeue());
        }
        for (int j = 0; j < CAPACITY / 2; j++)
        {
            assert(enqueue("cs50!"));
        }
    }
    printf("done!\n");

    printf("\n********\nSuccess!\n********\n");

    return 0;
}
