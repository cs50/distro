/*************************************************************************
 * sll.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Implements a simple singly-linked list structure for ints.
 *
 * QUESTIONS TO CONSIDER:
 * - why do we typically prepend instead of append to SLLs?
 * - why is it easier to insert a new node *after* instead of *before*
 *   another?
 * - what is the primary advantage of a SLL over an array?
 * - what is the primary advantage of an array over a SLL?
 * - with this SLL interface, what other data structure would be trivial
 *   to implement? (the stack)
 ************************************************************************/

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// the size of our test list: feel free to adjust as you wish!
#define TEST_SIZE 10

typedef struct node
{
    // the value to store in this node
    int i;

    // the link to the next node in the list
    struct node* next;
}
node;

// declare the first node of our list (as a global variable)
node* first = NULL;

/**
 * Returns the length of the list.
 */
int length(void)
{
    int length = 0;
    for (node* n = first; n != NULL; n = n->next)
    {
        length++;
    }
    return length;
}

/**
 * Returns true if a node in the list contains the value i and false
 * otherwise.
 */
bool contains(int i)
{
    for (node* n = first; n != NULL; n = n->next)
    {
        if (n->i == i)
        {
            return true;
        }
    }
    return false;    
}

/**
 * Helper function to build a new node. We'll use it in all of our
 * functions below that add nodes to the list.
 */
static node* build_node(int i)
{
    node* new_node = malloc(sizeof(node));
    if (new_node == NULL)
    {
        printf("sll: error: Out of heap memory\n");
        exit(1);
    }
    new_node->i = i;
    new_node->next = NULL;
    return new_node;
}

/**
 * Puts a new node containing i at the front (head) of the list.
 */
void prepend(int i)
{
    node* new_node = build_node(i);

    // the first node of our list will be the second node of our new one
    new_node->next = first;

    // our new node is now going to be the first node
    first = new_node;
}

/**
 * Inserts a new node containing int i following node n.
 */
static void insert_after(node* n, int i)
{
    if (n != NULL)
    {
        node* new_node = build_node(i);
        new_node->next = n->next;
        n->next = new_node;
    }
}

/**
 * Puts a new node containing i at the end (tail) of the list.
 */
void append(int i)
{
    // track the previous node to the current one as well, since it's
    // otherwise impossible to insert *before* the current node
    node* prev = NULL;
    for (node* cur = first; cur != NULL; cur = cur->next)
    {
        // make sure to update the prev pointer on every iteration!
        prev = cur;
    }

    // if prev is NULL, that means that the element belongs first
    if (prev == NULL)
    {
        prepend(i);
    }
    else
    {
        // aren't you glad that you already wrote this one? ;-)
        insert_after(prev, i);
    }
}

/**
 * Puts a new node containing i at the appropriate position in a list
 * sorted in ascending order.
 */
void insert_sorted(int i)
{
    // track the previous node to the current one as well, since it's
    // otherwise impossible to insert *before* the current node
    node* prev = NULL;
    for (node* cur = first; cur != NULL; cur = cur->next)
    {
        // break when we've found the first int larger than the one
        // we want to insert
        if (cur->i > i)
        {
            break;
        }

        // make sure to update the prev pointer on every iteration!
        prev = cur;
    }

    // if prev is NULL, that means that the element belongs first
    if (prev == NULL)
    {
        prepend(i);
    }
    else
    {
        // aren't you glad that you already wrote this one? ;-)
        insert_after(prev, i);        
    }
}

/**
 * Implements some simple test code for our singly-linked list.
 */
int main(void)
{
    printf("Prepending ints 0-%d onto the list...", TEST_SIZE);
    for (int i = 0; i < TEST_SIZE; i++)
    {
        prepend(i);
    }
    printf("done!\n");

    printf("Making sure that the list length is indeed %d...", TEST_SIZE);
    assert(length() == TEST_SIZE);
    printf("good!\n");

    printf("Making sure that values are arranged in descending order...");
    node* n = first;
    for (int i = 0; i < TEST_SIZE; i++)
    {
        assert(n != NULL);
        assert(n->i == TEST_SIZE - i - 1);
        n = n->next;
    }
    printf("good!\n");

    printf("Freeing the list...");
    while (first != NULL)
    {
        node* next = first->next;
        free(first);
        first = next;
    }
    printf("done!\n");

    printf("Appending ints 0-%d to the list...", TEST_SIZE);
    for (int i = 0; i < TEST_SIZE; i++)
    {
        append(i);
    }
    printf("done!\n");

    printf("Making sure that the list length is indeed %d...", TEST_SIZE);
    assert(length() == TEST_SIZE);
    printf("good!\n");

    printf("Making sure that values are arranged in ascending order...");
    n = first;
    for (int i = 0; i < TEST_SIZE; i++)
    {
        assert(n != NULL);
        assert(n->i == i);
        n = n->next;
    }
    printf("good!\n");

    printf("Freeing the list...");
    while (first != NULL)
    {
        node* next = first->next;
        free(first);
        first = next;
    }
    printf("done!\n");

    printf("Inserting %d random ints to the list...", TEST_SIZE);
    for (int i = 0; i < TEST_SIZE; i++)
    {
        insert_sorted(rand() % TEST_SIZE);
    }
    printf("done!\n");

    printf("Making sure that the list length is indeed %d...", TEST_SIZE);
    assert(length() == TEST_SIZE);
    printf("good!\n");

    printf("Making sure that values are arranged in sorted order...");
    n = first;
    int prev = 0;
    for (int i = 0; i < TEST_SIZE; i++)
    {
        assert(n != NULL);
        assert(n->i >= prev);
        prev = n->i;
        n = n->next;
    }
    printf("good!\n");

    printf("Freeing the list...");
    while (first != NULL)
    {
        node* next = first->next;
        free(first);
        first = next;
    }
    printf("done!\n");

    printf("\n********\nSuccess!\n********\n");

    return 0;
}
