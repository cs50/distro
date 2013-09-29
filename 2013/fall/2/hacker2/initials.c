/**
 * initials.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Prints user's initials based on user's name.
 *
 * Ignores leading and trailing spaces as well as multiple spaces in a row.
 */

#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    // get user's name
    string s = GetString();

    // ensure GetString didn't return NULL
    if (s != NULL)
    {
        // let's plan to output the first letter as an initial
        bool output = true;

        // iterate over characters in user's name
        for (int i = 0, n = strlen(s); i < n; i++)
        {
            // if it's time to output an initial, capitalize and output it
            if (output == true && isalpha(s[i]))
            {
                printf("%c", toupper(s[i]));
                output = false;
            }

            // if we've encountered a space, output the next letter as an initial
            if (s[i] == ' ')
            {
                output = true;
            }
        }

        // move cursor to next line
        printf("\n");
    }
}
