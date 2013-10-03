/***************************************************************************
 * generate.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Generates pseudorandom numbers in [0,LIMIT), one per line.
 *
 * Usage: generate n [s]
 *
 * where n is number of pseudorandom numbers to print
 * and s is an optional seed
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "helpers.h"

#define LIMIT 65536

int main(int argc, string argv[])
{
    // quits program and displays usage if the number of random numbers to
    // generate and an optional seed were not entered at the command line
    if (argc != 2 && argc != 3)
    {
        printf("Usage: generate n [s]\n");
        return 1;
    }

    // Converts desired number of random numbers from a string to an int
    int n = atoi(argv[1]);

    // if a seed was entered at the command line, use that for srand, else
    // just time(NULL)
    if (argc == 3)
    {
        srand((unsigned int) atoi(argv[2]));
    }
    else
    {
        srand((unsigned int) time(NULL));
    }

    // prints the desired number of random numbers, all 0 <= and < LIMIT
    for (int i = 0; i < n; i++)
    {
        printf("%d\n", rand() % LIMIT);
    }

    // that's all folks
    return 0;
}
