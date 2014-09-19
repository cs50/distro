/**
 * vigenere_2.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Encrypts user-supplied plaintext using a Vigenère cipher.
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
    // check for proper cmd line args
    if (argc != 2)
    {
        printf("Usage: %s <keyword>\n", argv[0]);
        return 1;
    }
    
    // use a nicer name than "argv[1]"
    string keyword = argv[1];
    int keyword_length = strlen(keyword);

    // ensure keyword is valid
    for (int i = 0; i < keyword_length; i++)
    {
        if (!isalpha(keyword[i]))
        {
            printf("Keyword must only contain letters A-Z and a-z\n");
            return 1;
        }
    }

    // get plaintext message from user; don't accept EOF as valid input
    string message;
    do
    {
        message = GetString();
    }
    while (message == NULL);

    // encrypt the plaintext in place, tracking letters seen to skip spaces
    int num_letters_seen = 0;
    for (int i = 0; i < strlen(message); i++)
    {
        // only encrypt letters, not other chars (e.g., digits)
        if (isalpha(message[i]))
        {
            // turn our keyword letter into an integer key in range [0, 25)
            int key;
            if (isupper(keyword[num_letters_seen % keyword_length]))
            {
                key = keyword[num_letters_seen % keyword_length] - 'A';
            }
            else
            {
                key = keyword[num_letters_seen % keyword_length] - 'a';
            }

            // encrypt the current message letter just like a Caesar cipher
            if (isupper(message[i]))
            {
                printf("%c", ((message[i] - 'A' + key) % NUM_LETTERS) + 'A');
            }
            else
            {
                printf("%c", ((message[i] - 'a' + key) % NUM_LETTERS) + 'a');
            }
            
            num_letters_seen++;
        }
        else
        {
            printf("%c", message[i]);
        }
    }
    printf("\n");

    return 0;
}

