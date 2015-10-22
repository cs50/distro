/**
 * dictionary.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Implements a dictionary's functionality using a trie.
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dictionary.h"

// number of children per node
#define CHILDREN 27

// a node
typedef struct node
{
    bool word;
    struct node* children[CHILDREN];
}
node;

// trie
node trie;

// number of words in dictionary
unsigned int words = 0;

/**
 * Hashes character, returning value in [0,26].
 * Assumes word is alphabetical or apostrophe.
 */
int hash(char c)
{
    if (isalpha(c))
    {
        return tolower(c) - 'a';
    }
    else
    {
        return 26;
    }
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // start at root of trie
    node* n = &trie;

    // hash word one character at a time
    for (int i = 0, len = strlen(word); i < len; i++)
    {
        // hash i'th character of word
        int index = hash(word[i]);

        // index into trie
        if (n->children[index] == NULL)
        {
            return false;
        }
        n = n->children[index];
    }
    
    // return whether n marks the end of a word
    return n->word;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // initialize trie
    trie.word = false;
    for (int i = 0; i < CHILDREN; i++)
    {
        trie.children[i] = NULL;
    }

    // open dictionary
    FILE* file = fopen(dictionary, "r");
    if (file == NULL)
    {
        return false;
    }

    // buffer for a word
    char word[LENGTH + 1];

    // iterate over words in dictionary
    while (fscanf(file, "%s", word) != EOF)
    {
        // allocate node
        node* n = malloc(sizeof(node));
        if (n == NULL)
        {
            unload();
            return false;
        }

        // TODO: insert node into trie
 
        // update hash table's size
        words++;
    }

    // close file
    fclose(file);

    // success
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return words;
}


/**
 * Helper function for unload that recurses over node's descendants. 
 */
void helper(node* n)
{
    if (n == NULL)
    {
        return;
    }
    for (int i = 0; i < CHILDREN; i++)
    {
        helper(n->children[i]);
    }
    free(n);
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    for (int i = 0; i < CHILDREN; i++)
    {
        helper(trie.children[i]);
    }
    return true;
}
