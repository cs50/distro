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

    // take as many 25-cent bites out of problem as possible, relying on integer division for whole number
    coins += cents / 25;

    // amount of changed owed now equals remainder after dividing by 25
    cents = cents % 25;

    // repeat for dimes
    coins += cents / 10;
    cents = cents % 10;

    // repeat for nickels
    coins += cents / 5;
    cents = cents % 5;

    // remainder is now < 5, so handle with pennies 
    coins += cents;

    // print counter
    printf("%i\n", coins);
}
