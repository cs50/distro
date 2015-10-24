/**
 * dictionary.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Implements a dictionary's functionality using a hash table.
 */

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

// number of buckets in hash table
#define BUCKETS 26

// a node
typedef struct node
{
    char word[LENGTH + 1];
    struct node* next;
}
node;

// hash table
node* table[BUCKETS];

// number of words in dictionary
unsigned int words = 0;

/**
 * Hashes word using its first character, returning value in [0,25].
 * Assumes word is alphabetical and of length >= 1.
 */
int hash(const char* word)
{
    return tolower(word[0]) - 'a';
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // hash word
    int index = hash(word);

    // index into hash table
    node* current = table[index];

    // iterate over chain, looking for word
    while (current != NULL)
    {
        if (strcasecmp(current->word, word) == 0)
        {
            return true;
        }
        current = current->next;
    }

    // is not in dictionary
    return false;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // initialize hash table
    for (int i = 0; i < BUCKETS; i++)
    {
        table[i] = NULL;
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
        node* new = malloc(sizeof(node));
        if (new == NULL)
        {
            unload();
            return false;
        }

        // copy word into node
        strcpy(new->word, word);

        // insert node into hash table at start of a chain
        int index = hash(word);
        new->next = table[index];
        table[index] = new;

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
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    for (int i = 0; i < BUCKETS; i++)
    {
        while (table[i] != NULL)
        {
            node* next = table[i]->next;
            free(table[i]);
            table[i] = next;
        }
    }
    return true;
}
