/****************************************************************************
 * dictionary.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Implements a dictionary's functionality, using a hash table for the 
 * dictionary.
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

#define HASHTABLE_SIZE 15500

/**
 * Each node will store a pointer to the word and a pointer to the next node
 * with the same hash value (or NULL, if no such node follows).
 *
 * To be more memory-efficient, we could actually store the character data
 * inside this struct, as shown here:
 *
 * typedef struct node
 * {
 *     struct node* next;
 *     char word[];
 * }
 * node;
 *
 * char* word = "CS50 rocks!";
 * node* entry = malloc(sizeof(node) + strlen(word) + 1);
 * strcpy(entry->word, word);
 *
 * Cool beans! We'll do it the "easy" way though for students.
 */
typedef struct node
{
    char* word;
    struct node* next;
}
node;

// Global dictionary hashtable
node* g_table[HASHTABLE_SIZE];

// Global dictionary's size
int g_table_size = 0;

/**
 * Returns a hash value for word, in the range [0, HASHTABLE_SIZE - 1]
 * Lowercases characters before hashing to be case-insensitive
 * Hash function adapted from:
 *     http://stackoverflow.com/questions/98153/#98179
 */
static int hash_word(const char* word)
{
    /* magic numbers from http://www.isthe.com/chongo/tech/comp/fnv/ */
    size_t InitialFNV = 2166136261U;
    size_t FNVMultiple = 16777619;
    
    /* Fowler / Noll / Vo (FNV) Hash */
    size_t hash = InitialFNV;
    for (int i = 0; word[i] != '\0'; i++)
    {
        // xor the low 8 bits of the hash
        hash ^= (tolower(word[i]));

        // multply by the magic number
        hash *= FNVMultiple;
    }
    return (hash % HASHTABLE_SIZE);
}

/**
 * Builds a new hash table node with a deep copy of the provided word.
 * Returns NULL on failure (out of heap memory).
 */
static node* build_node(const char* word)
{
    // allocate memory for a node
    node* n = malloc(sizeof(node));
    if (n == NULL)
    {
        return NULL;
    }

    // make a persistent heap copy of the word
    n->word = strdup(word);
    if (n->word == NULL)
    {
        free(n);
        return NULL;
    }

    return n;
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{   
    // find the bucket in the table where the word should be
    node* bucket = g_table[hash_word(word)];

    // look at all entries in the bucket to see if the word's there
    for (node* entry = bucket; entry != NULL; entry = entry->next)
    {
        // use strcasecmp since words in dictionary are in lowercase
        if (strcasecmp(entry->word, word) == 0)
        {
            return true;
        }
    }
    return false;
}

/**
 * Loads dict into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // open the dict and check for errors
    FILE* fp = fopen(dictionary, "r");
    if (fp == NULL)
    {
        return false;
    }
    
    // loop through words until the end of the file
    char buffer[LENGTH + 1];
    while (fscanf(fp, "%45s", buffer) == 1)
    {        
        node* entry = build_node(buffer);
        if (entry == NULL)
        {
            unload();
            fclose(fp);
            return false;
        }

        // hash the word
        unsigned int hash = hash_word(buffer);

        // point next at the old entry in the hash table at this hash value
        entry->next = g_table[hash];
        
        // make this the new entry in the hash table for this hash value
        g_table[hash] = entry;
        
        // update dictionary size
        g_table_size++;
    }
    
    // close the dictionary
    fclose(fp);
    
    // load was successful
    return true;
}


/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return g_table_size;
}


/*
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    // loop through each hash table entry
    for (int i = 0; i < HASHTABLE_SIZE; i++)
    {
        // free all the nodes for words with this hash value
        node* entry = g_table[i];
        while (entry != NULL)
        {
            // make sure to save the next pointer prior to freeing!
            node* next = entry->next;

            // free both the word and the entry
            free(entry->word);
            free(entry);

            entry = next;
        }
    }

    return true;
}
