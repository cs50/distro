/*************************************************************************
 * stack.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Implements a simple stack structure for char* s.
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
    // storage for the elements in the stack
    char* strings[CAPACITY];

    // the number of elements currently in the stack
    int size;
}
stack;

// declare a stack (as a global variable)
stack s;

/**
 * Puts a new element into the stack onto the "top" of the data structure
 * so that it will be retrived prior to the elements already in the stack.
 *
 * Nice one-liner: s.strings[s.size++] = string;
 */
bool push(char* string)
{
    // if we're at capacity, we can't add anything more to our stack
    if (s.size == CAPACITY)
    {
        return false;
    }

    // add the new element to our stack
    s.strings[s.size] = string;

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
char* pop()
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
    printf("Pushing %d strings onto the stack...", CAPACITY);
    for (int i = 0; i < CAPACITY; i++)
    {
        char str[12];
        sprintf(str, "%d", i);
        push(strdup(str));
    }
    printf("done!\n");

    printf("Making sure that the stack size is indeed %d...", CAPACITY);
    assert(s.size == CAPACITY);
    printf("good!\n");

    printf("Making sure that push() now returns false...");
    assert(!push("too much!"));
    printf("good!\n");

    printf("Popping everything off of the stack...");
    char* str_array[CAPACITY];
    for (int i = 0; i < CAPACITY; i++)
    {
        str_array[i] = pop();
    }
    printf("done!\n");

    printf("Making sure that pop() returned values in LIFO order...");
    for (int i = 0; i < CAPACITY; i++)
    {
        char str[12];
        sprintf(str, "%d", CAPACITY - i - 1);
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

    return 0;
}
