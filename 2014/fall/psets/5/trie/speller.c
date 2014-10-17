/****************************************************************************
 * speller.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a spell-checker.
 ***************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <time.h>

#include "dictionary.h"

// default dictionary
#define DICTIONARY "/home/cs50/pset5/dictionaries/large"

int main(int argc, char* argv[])
{
    // check for correct number of args
    if (argc != 2 && argc != 3)
    {
        printf("Usage: speller [dictionary] text\n");
        return 1;
    }

    // processor times
    clock_t before, after;

    // benchmarks
    clock_t time_load = 0, time_check = 0, time_size = 0, time_unload = 0;

    // determine dictionary to use
    char* dictionary = (argc == 3) ? argv[1] : DICTIONARY;

    // load dictionary
    before = clock();
    bool loaded = load(dictionary);
    after = clock();

    // abort if dictionary not loaded
    if (!loaded)
    {
        printf("Could not load %s.\n", dictionary);
        return 1;
    }

    // calculate time to load dictionary
    time_load = after - before;

    // try to open text
    char* text = (argc == 3) ? argv[2] : argv[1];
    FILE* fp = fopen(text, "r");
    if (fp == NULL)
    {
        printf("Could not open %s.\n", text);
        unload();
        return 1;
    }

    // prepare to report misspellings
    printf("\nMISSPELLED WORDS\n\n");

    // prepare to spell-check
    int index = 0, misspellings = 0, words = 0;
    char word[LENGTH+1];

    // spell-check each word in text
    for (int c = fgetc(fp); c != EOF; c = fgetc(fp))
    {
        // allow only alphabetical characters and apostrophes
        if (isalpha(c) || (c == '\'' && index > 0))
        {
            // append character to word
            word[index] = c;
            index++;

            // ignore alphabetical strings too long to be words
            if (index > LENGTH)
            {
                // consume remainder of alphabetical string
                while ((c = fgetc(fp)) != EOF && isalpha(c));

                // prepare for new word
                index = 0;
            }
        }

        // ignore words with numbers (like MS Word can)
        else if (isdigit(c))
        {
            // consume remainder of alphanumeric string
            while ((c = fgetc(fp)) != EOF && isalnum(c));

            // prepare for new word
            index = 0;
        }

        // we must have found a whole word
        else if (index > 0)
        {
            // terminate current word
            word[index] = '\0';

            // update counter
            words++;

            // check word's spelling
            before = clock();
            bool misspelled = !check(word);
            after = clock();

            // update benchmark
            time_check += after - before;

            // print word if misspelled
            if (misspelled)
            {
                printf("%s\n", word);
                misspellings++;
            }

            // prepare for next word
            index = 0;
        }
    }

    // check whether there was an error
    if (ferror(fp))
    {
        fclose(fp);
        printf("Error reading %s.\n", text);
        unload();
        return 1;
    }

    // close text
    fclose(fp);

    // determine dictionary's size
    before = clock();
    unsigned int n = size();
    after = clock();

    // calculate time to determine dictionary's size
    time_size = after - before;

    // unload dictionary
    before = clock();
    bool unloaded = unload();
    after = clock();

    // abort if dictionary not unloaded
    if (!unloaded)
    {
        printf("Could not unload %s.\n", dictionary);
        return 1;
    }

    // calculate time to unload dictionary
    time_unload = after - before;

    // report benchmarks
    printf("\nWORDS MISSPELLED:     %i\n", misspellings);
    printf("WORDS IN DICTIONARY:  %i\n", n);
    printf("WORDS IN TEXT:        %i\n", words);
    printf("TIME IN load:         %li\n", time_load);
    printf("TIME IN check:        %li\n", time_check);
    printf("TIME IN size:         %li\n", time_size);
    printf("TIME IN unload:       %li\n", time_unload);
    printf("TIME IN TOTAL:        %li\n\n", 
     time_load + time_check + time_size + time_unload);

    // that's all folks
    return 0;
}
