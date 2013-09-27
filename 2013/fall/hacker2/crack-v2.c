/**
 * crack.c
 *
 * Rob Bowden <rob@cs.harvard.edu>
 *
 * Attempts to find the password that, when encrypted with the crypt function,
 * gives the encrypted string passed in at the command line. Uses just a brute-
 * force approach.
 */

#define _XOPEN_SOURCE

#include <stdio.h>
#include <string.h>
#include <cs50.h>
#include <unistd.h>

#define ENCRYPT_LENGTH 13
#define SALT_LENGTH 2

#define MAX_LENGTH 8
#define MIN_SYMBOL ' '
#define MAX_SYMBOL '~'

int main(int argc, string argv[])
{
    // make sure the user entered an encrypted password at the command line
    if (argc != 2)
    {
        printf("usage: %s encrypted-string\n", argv[0]);
        return 1;
    }

    // use a more intuitive name for argv[1]
    string encrypted = argv[1];

    // a password encrypted with 'crypt' should be 13 characters long
    if (strlen(encrypted) != ENCRYPT_LENGTH)
    {
        printf("Invalid encrypted string\n");
        return 2;
    }

    // the first two characters of the encrypted password represent the salt
    char salt[SALT_LENGTH + 1] = {0};
    strncpy(salt, encrypted, SALT_LENGTH);

    // we'll use guess to iterate over all possible 8-character passwords
    char guess[MAX_LENGTH + 1] = {MIN_SYMBOL, 0};
    
    // we should never break out of this loop. either we'll find the
    // password, or return when we've exhausted all possible options
    while (1)
    {

        // encrypt our current guess
        string encrypted_guess = crypt(guess, salt);

        // 'guess' is our password if the encrypted_guess
        // matches the given encrypted string
        if (strcmp(encrypted, encrypted_guess) == 0)
        {
            printf("%s\n", guess);
            return 0;
        }

        // we want to "increment" our guess to the next valid password.
        // the idea here is that we continuously increment the first character
        // in the array. when it reaches the largest valid character (~), the
        // first character wraps back around to a ' ' and we increment the
        // second character. If the second character wraps back around, we
        // increment the third character. And so on. If all the current
        // characters in the string were the max character, then the string
        // length will be increased by 1.
        int idx = 0;
        while (guess[idx] == MAX_SYMBOL)
        {
            guess[idx] = MIN_SYMBOL;
            idx++;
        }

        // if we broke out of the while loop by hitting the final \0 in idx,
        // then we've tried all possible strings, and none of them were correct
        if (idx > MAX_LENGTH)
        {
            printf("Couldn't find a password that encrypts to %s\n", encrypted);
            return 3;
        }

        // actually increment the character found in the while loop above
        if (guess[idx] == '\0')
	{
	    guess[idx] = MIN_SYMBOL;
	}
	else
	{
	    guess[idx]++;
	}
    }

    // can never reach here
}
