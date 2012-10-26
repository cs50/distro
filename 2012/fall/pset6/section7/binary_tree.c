/*************************************************************************
 * binary_tree.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Simple binary tree implementation.
 ************************************************************************/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct node
{
    int value;
    struct node* left_child;
    struct node* right_child;
}
node;

// global variable to hold the root of our tree
node* root = NULL;

/**
 * Returns true if the tree contains value and false otherwise.
 */
bool contains(int value)
{
    node* cur = root;
    while (cur != NULL)
    {
        if (cur->value == value)
        {
            return true;
        }
        else if (cur->value > value)
        {
            cur = cur->left_child;
        }
        else
        {
            cur = cur->right_child;
        }
    }
    return false;
}

/**
 * Inserts a new node containing value into the tree. Returns true if the
 * insert succeeded and false otherwise (e.g., insufficient heap memory,
 * value already in the tree).
 */
bool insert(int value)
{
    // look for the appropriate place in the tree
    node* parent = NULL;
    node* cur = root;
    while (cur != NULL)
    {
        parent = cur;
        if (cur->value == value)
        {
            return false;
        }
        else if (cur->value > value)
        {
            cur = cur->left_child;
        }
        else
        {
            cur = cur->right_child;
        }
    }

    // build a new node
    node* new_node = malloc(sizeof(node));
    if (new_node == NULL)
    {
        return false;
    }
    new_node->value = value;

    // put it into the tree
    if (parent->value > new_node->value)
    {
        parent->left_child = new_node;
    }
    else
    {
        parent->right_child = new_node;
    }

    return true;
}

int main(int argc, const char* argv[])
{
    root = malloc(sizeof(node));
    root->value = 7;
    root->left_child = NULL;
    root->right_child = NULL;

    node* three = malloc(sizeof(node));
    three->value = 3;
    three->left_child = NULL;
    three->right_child = NULL;

    node* six = malloc(sizeof(node));
    six->value = 6;
    six->left_child = NULL;
    six->right_child = NULL;

    node* nine = malloc(sizeof(node));
    nine->value = 9;
    nine->left_child = NULL;
    nine->right_child = NULL;

    root->left_child = three;
    root->right_child = nine;
    three->right_child = six;

    printf("Tree contains 6? %s\n", contains(6)? "true" : "false");
    printf("Tree contains 10? %s\n", contains(10)? "true" : "false");
    printf("Tree contains 1? %s\n", contains(1)? "true" : "false");
    printf("Tree contains 9? %s\n", contains(9)? "true" : "false");

    for (int i = 0; i < 10; i++)
    {
        printf("Inserting %d...%s!\n", i, insert(i)? "success" : "fail");
    }

    for (int i = 0; i < 10; i++)
    {
        printf("Tree contains %d? %s\n", i, contains(i)? "true" : "false");
    }

    // ignore freeing memory for now

    return 0;
}

