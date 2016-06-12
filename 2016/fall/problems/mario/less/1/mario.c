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
        height = get_int();
    }
    while (height < 0 || height > MAX_HEIGHT);

    // calculate pyramid's width
    int width = height + 1;

    // iterate over pyramid's rows
    for (int i = 0; i < height; i++)
    {
        // print spaces
        for (int j = 0; j < width - 2 - i; j++)
        {
            printf(" ");
        }
        
        // print hashes
        for (int j = 0; j < i + 2; j++)
        {
            printf("#");
        }

        // print newline
        printf("\n");
    }
}
