#include <cs50.h>
#include <stdio.h>

int main(void)
{
    // prompt user for positive integer
    int n;
    do
    {
        printf("Minutes: ");
        n = get_int();
    }
    while (n < 1);
    
    // report corresponding number of bottles
    printf("Bottles: %i\n", n * 12);
}
