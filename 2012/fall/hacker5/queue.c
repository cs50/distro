/*************************************************************************
 * queue.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Implements a simple queue structure for char* s. This version uses
 * dynamic, heap-allocated storage for the char* s instead of a
 * statically-sized stack array.
 ************************************************************************/

// for strdup() in the testing code
//#define _XOPEN_SOURCE 500

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 5
#define TEST_CAPACITY 10

typedef struct
{
    // the index of the first element in the queue
    int head;

    // dynamic storage for the elements in the queue
    char** strings;

    // the number of elements in the queue
    int size;

    // the allocated capacity of the queue
    int capacity;
}
queue;

// declare a queue (as a global variable)
queue q;

/**
 * Expands the queue's capacity to twice its existing capacity. Returns
 * false if the expansion failed due to lack of heap memory, otherwise
 * returns true.
 *
 * The trick here is that we can't just realloc(capacity * 2) and call it
 * a day. Here's why. If we had a queue of capacity 5 and we'd made a
 * series of calls like the following:
 *
 * enqueue("a"); enqueue("b"); enqueue("c"); enqueue("d"); enqueue("e");
 * dequeue(); dequeue();
 * enqueue("f"); enqueue("g");
 *
 * then our queue will look like the below:
 *
 * +-----------------------------+
 * | "f" | "g" | "c" | "d" | "e" |
 * +-----------------------------+
 *                ^
 *               head
 *
 * When we expand, we can't just extend the length, as that would throw off
 * our math when adjusting the head "pointer." Instead, we need to
 * rearrange the queue to look as follows:
 *
 * +-----------------------------------------------------------+
 * | "c" | "d" | "e" | "f" | "g" |     |     |     |     |     |
 * +-----------------------------------------------------------+
 *    ^
 *   head
 */
bool expand(void)
{
    // double the queue's capacity
    q.capacity *= 2;

    // create a new array of elements for the queue
    // don't use realloc since we'll need to manually shift elems over
    char** new_strings = malloc(q.capacity * sizeof(char*));
    if (new_strings == NULL)
    {
        return false;
    }

    // copy the "first" elements in our queue to the front of the new array
    memcpy(new_strings, q.strings + q.head, (q.size - q.head) * sizeof(char*));

    // copy the "last" elements in our queue to the end of the new array
    memcpy(new_strings + q.head, q.strings, q.head * sizeof(char*));

    // now reset the queue's array
    free(q.strings);
    q.strings = new_strings;
    
    // reset our head pointer
    q.head = 0;

    return true;
}

/**
 * Puts a new element into the queue into the "end" of the data structure
 * so that it will be retrived after the other elements already in the
 * queue.
 *
 * One-liner: q.strings[(q.head + q.size++) % CAPACITY] = str;
 */
bool enqueue(char* str)
{
    // if we're at capacity, expand!
    if (q.size == q.capacity)
    {
        if (!expand())
        {
            return false;
        }
    }

    // calculate the index of the "last" slot in the queue
    int last = (q.head + q.size) % q.capacity;

    // add the new element to our queue
    q.strings[last] = str;

    // adjust the size appropriately
    q.size++;

    return true;
}

/**
 * Retrieves ("dequeues") the first element in the queue, following the
 * the "first-in, first-out" (FIFO) ordering of the data structure.
 * Reduces the size of the queue and adjusts the head to the next element.
 *
 * No one-liner here since we can't do a postfix mod and the dequeue
 * doesn't depend on the size of the queue anyway!
 */
char* dequeue(void)
{
    // if nothing's in our queue, we can't retrieve anything!
    if (q.size == 0)
    {
        return NULL;
    }

    // grab the first element in our queue
    char* first = q.strings[q.head];

    // adjust the head of our queue to point to the next element
    // wrap back to the beginning if necessary
    q.head = (q.head + 1) % q.capacity;

    // adjust the size appropriately
    q.size--;

    return first;
}

void print_queue(void)
{
    printf("\nQUEUE IS NOW: ");
    for (int i = 0; i < q.capacity; i++)
    {
        if ((q.head <= i && i < q.head + q.size) ||
            i < ((q.head + q.size) % q.capacity))
        {
            printf("\"%s\" ", q.strings[i]);
        }
        else
        {
            printf("\"???\" ");
        }
    }
    printf("\n\n");
}

/**
 * Implements some simple test code for our queue
 */
int main(void)
{
    // initialize the queue
    q.head = 0;
    q.strings = malloc(INITIAL_CAPACITY * sizeof(char*));
    q.size = 0;
    q.capacity = INITIAL_CAPACITY;

    printf("Enqueueing %d strings...", TEST_CAPACITY);
    for (int i = 0; i < TEST_CAPACITY; i++)
    {
        char str[12];
        sprintf(str, "%d", i);
        enqueue(strdup(str));
    }
    printf("done!\n");

    printf("Making sure that the queue size is indeed %d...", TEST_CAPACITY);
    assert(q.size == TEST_CAPACITY);
    printf("good!\n");

    printf("Dequeueing everything...");
    char* str_array[TEST_CAPACITY];
    for (int i = 0; i < TEST_CAPACITY; i++)
    {
        str_array[i] = dequeue();
    }
    printf("done!\n");

    printf("Making sure that dequeue() returned values in FIFO order...");
    for (int i = 0; i < TEST_CAPACITY; i++)
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
    for (int i = 0; i < TEST_CAPACITY; i++)
    {
        enqueue("cs50!");
    }

    // to make sure that they adjust the head pointer appropriately, we'll
    // enqueue and dequeue a bunch of times to make sure they don't just
    // walk off the end of the char* strings[] array
    for (int i = 0; i < TEST_CAPACITY * 10; i++)
    {
        for (int j = 0; j < TEST_CAPACITY / 2; j++)
        {
            assert(dequeue());
        }

        for (int j = 0; j < TEST_CAPACITY / 2; j++)
        {
            enqueue("cs50!");
        }
    }
    printf("good!\n");

    // reinitialize the queue
    free(q.strings);
    q.head = 0;
    q.strings = malloc(INITIAL_CAPACITY * sizeof(char*));
    q.size = 0;
    q.capacity = INITIAL_CAPACITY;

    for (int i = 0; i < TEST_CAPACITY; i++)
    {
        char str[12];
        sprintf(str, "%d", i);
        enqueue(strdup(str));
    }

    for (int i = 0; i < TEST_CAPACITY / 2; i++)
    {
        assert(dequeue());
    }

    for (int i = 0; i < TEST_CAPACITY * 2; i++)
    {
        char str[12];
        sprintf(str, "%d", TEST_CAPACITY + i);
        enqueue(strdup(str));
    }

    for (int i = 0; i < TEST_CAPACITY * 2; i++)
    {
        assert(dequeue());
    }

    printf("\n********\nSuccess!\n********\n");

    // clean up
    free(q.strings);

    return 0;
}
