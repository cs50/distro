/**
 * mario.c
 *
 * Alex Hugon
 *
 * Takes a number for height and creates two "half-pyramids" like those from
 * Super Mario Bros.
 */

#include <cs50.h>
#include <stdio.h>

int main(void)
{
    // prompt the user for the height of the half pyramids
    int height = 0;
    do
    {
        printf("Enter a non-negative integer < 24: ");
        height = GetInt();
    }
    while (height > 23 || height < 0);

    // iterate through the rows of the pyramids
    for (int row = 1; row <= height; row++)
    {
        // number of spaces to print
        int spaces = height - row;

        // print first set of spaces
        for (int j = 0; j < spaces; j++)
        {
            printf(" ");
        }

        // print left left pyramid
        for (int j = 0; j < row; j++)
        {
            printf("#");
        }

        // print gap between pyramids
        printf("  ");

        // print right pyramid
        for (int j = 0; j < row; j++)
        {
            printf("#");
        }

        // move to next row
        printf("\n");
    }
}
