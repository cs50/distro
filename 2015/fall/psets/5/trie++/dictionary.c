/**
 * dictionary.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Implements a dictionary's functionality.
 */

#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dictionary.h"

// root of a trie
static struct node *root;

// number of words in trie
static unsigned int count;

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    register struct node *current = root;
    const register char *w = word;
    while (*w != '\0')
    {
        register unsigned int index = (*w == '\'') ? 26 : (*w | 32) - 'a';
        current = current->array[index];
        if (current == NULL) return false;
        w++;
    }
    return current->stop;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // pre-allocate nodes
    struct stat buf;
    if (stat(dictionary, &buf)) return false;

    // allocate root
    root = calloc(1, sizeof(struct node));
    if (root == NULL) return false;

    // map file into memory
    int fd = open(dictionary, O_RDONLY);
    if (fd < 0) return false;
    char *map = mmap(0, buf.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (map == MAP_FAILED) return false;

    // add words to trie
    for (register char *c = map, *eof = c + buf.st_size; c < eof; c++)
    {
        register struct node *current = root;
        while (*c != '\n')
        {
            register unsigned int index = (*c == '\'') ? 26 : *c - 'a';
            if (current->array[index] == NULL && ((current->array[index] = calloc(1, sizeof(struct node))) == NULL)) return false;
            current = current->array[index];
            c++;
        }
        current->stop = true;
        count++;
    }

    // close file
    close(fd);

    // that's all folks
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return count;
}

/*
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
void unloader(struct node* n)
{
    if (n != NULL)
    {
        for (int i = 0; i < CHARS; i++)
        {
            unloader(n->array[i]);
            if (n->array[i] != NULL)
                free(n->array[i]);
        }
    }
}
bool unload(void)
{
    unloader(root);
    free(root);
    return true;
}
