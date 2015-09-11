#include <cs50.h>
#include <stdio.h>

int main(void)
{
    // prompt user for positive integer
    int n;
    do
    {
        printf("minutes: ");
        n = GetInt();
    }
    while (n < 1);
    
    // report corresponding number of bottles
    printf("bottles: %i\n", n * 12);
}
