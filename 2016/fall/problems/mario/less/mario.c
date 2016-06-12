#include <cs50.h>
#include <stdio.h>

#define MAX_HEIGHT 23

int main(void)
{
    // prompt user for pyramid's height
    int height;
    do
    {
        printf("Height: ");
        height = GetInt();
    }
    while (height < 0 || MAX_HEIGHT < height);

    // build pyramid top-down (since you can't do bottom-up with printf)
    for (int row = height; row > 0; row--)
    {
        // when row == 1, pyramid should be flush against left side
        int spaces = row - 1;

        // print out the spaces first
        for (int j = 0; j < spaces; j++)
        {
            printf(" ");
        }

        // there are two ## at the top of a pyramid (when height == row)
        int hashes = height - row + 2;

        // now print out all of the hashes
        for (int j = 0; j < hashes; j++)
        {
            printf("#");
        }
        printf("\n");
    }
}
