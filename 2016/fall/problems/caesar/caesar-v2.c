/**
 * caesar_2.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 * Doug Lloyd <lloyd@cs50.harvard.edu>
 *
 * Encrypts user-supplied plaintext using a Caesar cipher.
 */

#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// there are 26 letters in the English alphabet!
#define NUM_LETTERS 26

int main(int argc, string argv[])
{
    // check for proper command line arguments
    if (argc != 2)
    {
        printf("Usage: %s <key>\n", argv[0]);
        return 1;
    }

    // ensure key is in range [0, inf)
    int key = atoi(argv[1]);
    if (key < 0)
    {
        printf("Key must be a non-negative integer.\n");
        return 1;
    }

    // get plaintext message from user; don't accept EOF as valid input
    printf("plaintext: ");
    string message;
    do
    {
        message = get_string();
    }
    while (message == NULL);

    // encrypt the plaintext, printing out one char at a time
    // strlen can get optimized out by the compiler here since we're not
    // modifying the contents of "message."
    printf("ciphertext: ");
    for (int i = 0; i < strlen(message); i++)
    {
        // only encrypt letters, not other chars (e.g., digits)
        if (isupper(message[i]))
        {
            printf("%c", ((message[i] - 'A' + key) % NUM_LETTERS) + 'A');
        }
        else if (islower(message[i]))
        {
            printf("%c", ((message[i] - 'a' + key) % NUM_LETTERS) + 'a');
        }
        else
        {
            printf("%c", message[i]);
        }
    }

    // now "message" holds encrypted ciphertext
    printf("\n");

    return 0;
}

