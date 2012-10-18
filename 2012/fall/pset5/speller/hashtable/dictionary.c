/****************************************************************************
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 6
 *
 * Josh Bolduc
 *
 * Implements a dictionary's functionality.
 ***************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"


// Global variable to keep track of the dictionary's size
int g_dict_size = 0;

// Global hashtable
node *g_table[HASHTABLE_SIZE];

// Pointer to chunk of memory where we'll store all the words in the dict
char *g_words;


/*
 * Returns true if word is in dictionary else false.
 */

bool
check(const char *word)
{
    // make lowercase copy of word in temporary buffer
    char word_i[LENGTH+1];
    int len;
    for (len = 0; word[len] != '\0'; ++len)
    {
        word_i[len] = tolower(word[len]);
    }
    word_i[len] = '\0';
    
    // get the hash value for the lowercase word
    int hash = hash_word(word_i, NULL);

    // see if any word in the dict has this hash value
    if (g_table[hash] == NULL)
    {
        return false;
    }
    
    // see if the word was actually in the dict, looping until
    // we find it (or NULL)
    node * ptr = g_table[hash];
    while (strcmp(ptr->word, word_i) != 0)
    {
        // NULL means no more words to check, so no match
        if (ptr->next == NULL)
        {
            return false;
        }
        ptr = ptr->next;
    }
    
    // if we got here without returning false, we must have found the word
    return true;
}


/*
 * Loads dict into memory.  Returns true if successful else false.
 */

bool
load(const char *dictionary)
{
    // open the dict and check for errors
    FILE * fp = fopen(dictionary, "r");
    if (fp == NULL)
    {
        return false;
    }
    
    // determine dict size in bytes
    struct stat stats;
    if (stat(dictionary, &stats))
    {
        fclose(fp);
        return false;
    }
    
    // allocate enough memory to hold all the words
    // (we'll assume the dict filesize won't change while we're running)
    g_words = malloc(stats.st_size);
    if (g_words == NULL)
    {
        fclose(fp);
        return false;
    }
    
    // initialize all pointers in the hashtable to NULL
    for (int i = 0; i < HASHTABLE_SIZE; ++i)
    {
        g_table[i] = NULL;
    }
    
    // loop through words until the end of the file
    int mem_index = 0;
    while (!feof(fp))
    {
        // scan for a word and, if we find one, store it in our hash table
        if (fscanf(fp, "%s", &g_words[mem_index]) == 1)
        {
            // get both the hash value and string length
            int len;
            unsigned int hash = hash_word(&g_words[mem_index], &len);
            
            // allocate memory for this word's node
            node *newword = malloc(sizeof(node));
            if (newword == NULL)
            {
                free(g_words);
                fclose(fp);
                return false;
            }

            // point to the word we just read into memory
            newword->word = &g_words[mem_index];
            
            // point next at the old entry in the hash table at this hash value
            newword->next = g_table[hash];
            
            // make this the new entry in the hash table for this hash value
            g_table[hash] = newword;
            
            // update dictionary size
            ++g_dict_size;
            
            // update offset into our big chunk of memory
            mem_index += len+1;
        }
    }
    
    // close the dictionary
    fclose(fp);
    
    // load was successful
    return true;
}


/*
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */

unsigned int
size(void)
{
    return g_dict_size;
}


/*
 * Unloads dictionary from memory.  Returns true if successful else false.
 */

bool
unload(void)
{
    node *next;

    // loop through each hash table entry
    for (int i = 0; i < HASHTABLE_SIZE; ++i)
    {
        // free all the nodes for words with this hash value
        node *ptr = g_table[i];
        while (ptr != NULL)
        {
            next = ptr->next;
            free(ptr);
            ptr = next;
        }
    }
    
    // free the big chunk of memory that held all the words
    free(g_words);

    return true;
}


/*
 * Returns a hash value for word, in the range [0, HASHTABLE_SIZE - 1]
 * If len != NULL, puts the length of word in the address stored in len
 * Hash function adapted from:
 *     http://stackoverflow.com/questions/98153/#98179
 */
 
inline
int
hash_word(char *word, int *len)
{
    /* magic numbers from http://www.isthe.com/chongo/tech/comp/fnv/ */
    size_t InitialFNV = 2166136261U;
    size_t FNVMultiple = 16777619;
    
    /* Fowler / Noll / Vo (FNV) Hash */
    size_t hash = InitialFNV, i;
    for(i = 0; word[i] != '\0'; ++i)
    {
        hash = hash ^ (word[i]);    /* xor  the low 8 bits */
        hash = hash * FNVMultiple;  /* multiply by the magic number */
    }

    // if requested, return string length
    if (len)
        *len = i;

    return (hash % HASHTABLE_SIZE);
}
