#include <cs50.h>
#include <math.h>
#include <stdio.h>

int main(void)
{
    // prompt user for change owed
    float dollars;
    do
    {
        printf("Change: ");
        dollars = get_float();
    }
    while (dollars < 0);

    // convert dollars to cents to avoid floating-point imprecision
    int cents = round(dollars * 100);

    // initialize counter for coins
    int coins = 0;

    // take as many 25-cent bites out of problem as possible
    while (cents >= 25)
    {
        // increment counter by 1 (quarter)
        coins++;

        // subtract a quarter from remaining change
        cents -= 25;
    }

    // 10-cent bites 
    while (cents >= 10)
    {
        coins++;
        cents -= 10;
    }

    // 5-cent bites
    while (cents >= 5)
    {
        coins++;
        cents -= 5;
    }

    // 1-cent bites
    while (cents >= 1)
    {
        coins++;
        cents -= 1;
    }

    // print counter
    printf("%i\n", coins);
}
