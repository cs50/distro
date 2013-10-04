/**
 * capitalize.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Capitalizes user's name.
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
        // let's plan to capitalize the first letter   
        bool capitalize = true;

        // iterate over characters in user's name
        for (int i = 0, n = strlen(s); i < n; i++)
        {
            // if i'th character is alphabetical
            // and we should capitalize, capitalize
            if (isalpha(s[i]) && capitalize)
            {
                printf("%c", toupper(s[i]));
                capitalize = false;
            }

            // else print i'th character as is
            else
            {
                printf("%c", s[i]);

                // if i'th character is a space, let's plan
                // to capitalize next alphabetical character we see
                if (s[i] == ' ')
                {
                    capitalize = true;
                }
            }
        }

        // move cursor to next line
        printf("\n");
    }
}
