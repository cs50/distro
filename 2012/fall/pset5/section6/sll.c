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

#define TEST_SIZE 10

typedef struct node
{
    int i;
    struct node* next;
}
node;

// declare the first node of our list (as a global variable)
node* first = NULL;

/**
 * Helper function to build a new node. We'll use it in all of our
 * functions below that add nodes to the list.
 */
static node* build_node(int i)
{
    node* new_node = malloc(sizeof(node));
    new_node->i = i;
    new_node->next = NULL;
    return new_node;
}

/**
 * Prepends a new node containing int i to the list.
 */
void push(int i)
{
    node* new_node = build_node(i);

    // the first node of our list will be the second node of our new one
    new_node->next = first;

    // our new node is now going to be the first node
    first = new_node;
}

/**
 * Deletes the first node from the list, returning the int that it
 * contained. Note that the list *cannot* be empty!
 */
int pop()
{
    // pull the first node out of the list
    node* n = first;
    first = first->next;
 
    // remember the value the node is holding
    int i = n->i;

    // free the node
    free(n);

    return i;
}

/**
 * Inserts a new node containing int i following node n.
 */
void insert_after(node* n, int i)
{
    if (n != NULL)
    {
        node* new_node = build_node(i);
        new_node->next = n->next;
        n->next = new_node;
    }
}

/**
 * Attaches a new node containing int i to the end of the list.
 *
 * Note how this is a heck of a lot harder than just prepending,
 * and it takes O(n) time instead of O(1)!
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
        push(i);
    }
    else
    {
        // aren't you glad that you already wrote this one? ;-)
        insert_after(prev, i);
    }
}

/**
 * Inserts a new node containing int i into the list in sorted (ascending)
 * order.
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
        push(i);
    }
    else
    {
        // aren't you glad that you already wrote this one? ;-)
        insert_after(prev, i);        
    }
}

/**
 * Deletes the node following node n in the list.
 */
void delete_after(node* n)
{
    if (n != NULL)
    {
        node* to_delete = n->next;
        if (to_delete != NULL)
        {
            n->next = to_delete->next;
        }
        free(to_delete);
    }
}

/**
 * Determines whether or not a node in the list contains int i.
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
 * Finds the position of the first node in the list, if any, that contains
 * int i. Returns -1 on failure.
 */
bool position(int i)
{
    int position = 0;
    for (node* n = first; n != NULL; n = n->next)
    {
        if (n->i == i)
        {
            return position;
        }
        position++;
    }
    return -1;
}

/**
 * Determines the length of the linked list.
 */
int length()
{
    int length = 0;
    for (node* n = first; n != NULL; n = n->next)
    {
        length++;
    }
    return length;
}

/**
 * Iterates through the list one node at a time, printing each node's int.
 */
void print_list()
{
    printf("\nLIST IS NOW: ");
    for (node* n = first; n != NULL; n = n->next)
    {
        printf("%d ", n->i);
    }
    printf("\n\n");
}

/**
 * Implements some simple test code for our singly-linked list.
 */
int main(void)
{
    printf("Pushing ints 0-%d onto the list...", TEST_SIZE);
    for (int i = 0; i < TEST_SIZE; i++)
    {
        push(i);
    }
    printf("done!\n");

    printf("Making sure that the list size is indeed %d...", TEST_SIZE);
    assert(length() == TEST_SIZE);
    printf("good!\n");

    int array[TEST_SIZE];
    printf("Popping everything out of the list...");
    for (int i = 0; i < TEST_SIZE; i++)
    {
        array[i] = pop();
    }
    printf("done!\n");

    printf("Making sure that the list is now empty...");
    assert(length() == 0);
    printf("good!\n");

    printf("Making sure that values were returned in LIFO order...");
    for (int i = 0; i < TEST_SIZE; i++)
    {
        assert(array[i] == TEST_SIZE - i - 1);
    }
    printf("good!\n");

    printf("Appending ints 0-%d to the list...", TEST_SIZE);
    for (int i = 0; i < TEST_SIZE; i++)
    {
        append(i);
    }
    printf("done!\n");

    printf("Making sure that the list size is indeed %d...", TEST_SIZE);
    assert(length() == TEST_SIZE);
    printf("good!\n");

    printf("Popping everything out of the list...");
    for (int i = 0; i < TEST_SIZE; i++)
    {
        array[i] = pop();
    }
    printf("done!\n");

    printf("Making sure that the list is now empty...");
    assert(length() == 0);
    printf("good!\n");

    printf("Making sure that values were returned in FIFO order...");
    for (int i = 0; i < TEST_SIZE; i++)
    {
        assert(array[i] == i);
    }
    printf("good!\n");

    printf("Inserting %d random ints to the list...", TEST_SIZE);
    for (int i = 0; i < TEST_SIZE; i++)
    {
        insert_sorted(random() % TEST_SIZE);
    }
    printf("done!\n");

    printf("Making sure that the list size is indeed %d...", TEST_SIZE);
    assert(length() == TEST_SIZE);
    printf("good!\n");

    printf("Popping everything out of the list...");
    for (int i = 0; i < TEST_SIZE; i++)
    {
        array[i] = pop();
    }
    printf("done!\n");

    printf("Making sure that the list is now empty...");
    assert(length() == 0);
    printf("good!\n");

    printf("Making sure that values were returned in sorted order...");
    for (int i = 1; i < TEST_SIZE; i++)
    {
        assert(array[i] >= array[i - 1]);
    }
    printf("good!\n");

    printf("\n********\nSuccess!\n********\n");

    return 0;
}
