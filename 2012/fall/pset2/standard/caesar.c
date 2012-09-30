/**
 * caesar.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
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
    string message;
    do
    {
        message = GetString();
    }
    while (message == NULL);

    // encrypt the plaintext in place
    // strlen won't get optimized out of the condition here, since we're
    // modifying the string, so best to pull it into a separate variable!
    for (int i = 0, n = strlen(message); i < n; i++)
    {
        // only encrypt letters, not other chars (e.g., digits)
        if (isupper(message[i]))
            message[i] = ((message[i] - 'A' + key) % NUM_LETTERS) + 'A';
        else if (islower(message[i]))
            message[i] = ((message[i] - 'a' + key) % NUM_LETTERS) + 'a';
    }

    // now "message" holds encrypted ciphertext
    printf("%s\n", message);

    return 0;
}

