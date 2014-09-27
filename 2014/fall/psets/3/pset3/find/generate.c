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
 
// standard libraries
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// constant
#define LIMIT 65536

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2 && argc != 3)
    {
        printf("Usage: generate n [s]\n");
        return 1;
    }

    // convert command-line argument from string to integer
    int n = atoi(argv[1]);

    // seed pseudorandom-number generator
    if (argc == 3)
    {
        srand48((unsigned int) atoi(argv[2]));
    }
    else
    {
        srand48((unsigned int) time(NULL));
    }

    // generate pseudorandom integers, one per line
    for (int i = 0; i < n; i++)
    {
        printf("%i\n", (int) (drand48() * LIMIT));
    }

    // success
    return 0;
}
