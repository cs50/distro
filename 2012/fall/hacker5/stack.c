/*************************************************************************
 * stack.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Implements a simple stack structure for char* s. This version uses
 * dynamic, heap-allocated storage for the char* s instead of a
 * statically-sized stack array.
 ************************************************************************/

// for strdup() in the testing code
#define _XOPEN_SOURCE 500

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 5
#define TEST_CAPACITY 10

typedef struct
{
    // dynamic storage for the elements in the stack
    char** strings;

    // the number of elements currently in the stack
    int size;

    // the allocated capacity of the stack
    int capacity;
}
stack;

// declare a stack (as a global variable)
stack s;

/**
 * Expands the stack's capacity to twice its existing capacity. Returns
 * false if the expansion failed due to lack of heap memory, otherwise
 * returns true.
 */
bool expand(void)
{
    s.capacity *= 2;

    // make sure to capture the return value of realloc!
    s.strings = realloc(s.strings, s.capacity * sizeof(char*));

    return (s.strings != NULL);
}

/**
 * Puts a new element into the stack onto the "top" of the data structure
 * so that it will be retrived prior to the elements already in the stack.
 *
 * Nice one-liner: s.strings[s.size++] = str;
 */
bool push(char* str)
{
    // if we're at capacity, we can't add anything more to our stack
    if (s.size == s.capacity)
    {
        // if the expansion fails, notify caller
        if (!expand())
        {
            return false;
        }
    }

    // add the new element to our stack
    s.strings[s.size] = str;

    // adjust the size appropriately
    s.size++;

    return true;
}

/**
 * Retrieves ("pops") the last ("top") element off of the stack, following
 * the "last-in, first-out" (LIFO) ordering of the data structure. Reduces
 * the size of the stack.
 *
 * Nice one-liner: return s.strings[--s.size];
 */
char* pop(void)
{
    // if nothing's in our stack, we can't retrieve anything!
    if (s.size == 0)
    {
        return NULL;
    }

    // grab the last element in our stack
    char* last = s.strings[s.size - 1];

    // adjust the size accordingly
    s.size--;

    return last;
}

/**
 * Implements some simple test code for our stack
 */
int main(void)
{
    // initialize the stack
    s.size = 0;
    s.capacity = INITIAL_CAPACITY;
    s.strings = malloc(s.capacity * sizeof(char*));

    printf("Pushing %d strings onto the stack...", TEST_CAPACITY);
    for (int i = 0; i < TEST_CAPACITY; i++)
    {
        char str[12];
        sprintf(str, "%d", i);
        push(strdup(str));
    }
    printf("done!\n");

    printf("Making sure that the stack size is indeed %d...", TEST_CAPACITY);
    assert(s.size == TEST_CAPACITY);
    printf("good!\n");

    printf("Popping everything off of the stack...");
    char* str_array[TEST_CAPACITY];
    for (int i = 0; i < TEST_CAPACITY; i++)
    {
        str_array[i] = pop();
    }
    printf("done!\n");

    printf("Making sure that pop() returned values in LIFO order...");
    for (int i = 0; i < TEST_CAPACITY; i++)
    {
        char str[12];
        sprintf(str, "%d", TEST_CAPACITY - i - 1);
        assert(strcmp(str_array[i], str) == 0);
        free(str_array[i]);
    }
    printf("good!\n");

    printf("Making sure that the stack is now empty...");
    assert(s.size == 0);
    printf("good!\n");

    printf("Making sure that pop() now returns NULL...");
    assert(pop() == NULL);
    printf("good!\n");

    printf("\n********\nSuccess!\n********\n");

    // clean up
    free(s.strings);

    return 0;
}
