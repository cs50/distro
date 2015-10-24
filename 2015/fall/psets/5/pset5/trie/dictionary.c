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
node* trie = NULL;

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
    // look for word in trie
    node* current = trie;
    for (int i = 0, n = strlen(word); i < n; i++)
    {
        // hash i'th character of word
        int index = hash(word[i]);

        // index into trie
        if (current->children[index] == NULL)
        {
            return false;
        }
        current = current->children[index];
    }
    
    // return whether node marks the end of a word
    return current->word;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // allocate root of trie
    trie = malloc(sizeof(node));
    if (trie == NULL)
    {
        return false;
    }
    trie->word = false;
    for (int i = 0; i < CHILDREN; i++)
    {
        trie->children[i] = NULL;
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
        // insert word into trie
        node* current = trie;
        for (int i = 0, n = strlen(word); i < n; i++)
        {
            // hash i'th character of word
            int index = hash(word[i]);

            // index into trie
            if (current->children[index] == NULL)
            {
                // allocate node
                node* new = malloc(sizeof(node));
                if (new == NULL)
                {
                    unload();
                    return false;
                }

                // insert node into trie
                current->children[index] = new;
            }
            current = current->children[index];
        }

        // flag word as inserted
        current->word = true;
 
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
void helper(node* current)
{
    if (current == NULL)
    {
        return;
    }
    for (int i = 0; i < CHILDREN; i++)
    {
        helper(current->children[i]);
    }
    free(current);
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    helper(trie);
    return true;
}
