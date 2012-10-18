/****************************************************************************
 * dictionary.h
 *
 * Computer Science 50
 * Problem Set 6
 *
 * Josh Bolduc
 *
 * Declares a dictionary's functionality.
 ***************************************************************************/

#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>


// maximum length for a word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
#define LENGTH 45

// size of the hashtable
#define HASHTABLE_SIZE 155000


/*
 * Returns true if word is in dictionary else false.
 */

bool check(const char *word);


/*
 * Loads dictionary into memory.  Returns true if successful else false.
 */

bool load(const char *dictionary);


/*
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */

unsigned int size(void);


/*
 * Unloads dictionary from memory.  Returns true if successful else false.
 */

bool unload(void);


/*
 * Returns a hash value for word, in the range [0, HASHTABLE_SIZE - 1]
 * If len != NULL, puts the length of word in the address stored in len
 * Hash function adapted from:
 *     http://stackoverflow.com/questions/98153/#98179
 */
int hash_word(char *word, int *len);


/*
 * Each node will store a pointer to the word and a pointer to
 * the next node with the same hash value (or NULL, if no such
 * node follows.
 */
typedef struct _node
{
    char *word;
    struct _node *next; 
} node;



#endif // DICTIONARY_H
