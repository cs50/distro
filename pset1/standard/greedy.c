/**
 * greedy.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Uses a greedy algorithm to determine how many coins are necessary to make
 * change for a user-provided dollar amount.
 */

#include <cs50.h>
#include <math.h>
#include <stdio.h>

#define CENTS_PER_DOLLAR 100
#define QUARTER 25
#define DIME 10
#define NICKEL 5
#define PENNY 1

int main(void)
{
    float dollars;
    do
    {
        printf("O hai! How much change is owed?\n");
        dollars = GetFloat();
    }
    while (dollars < 0);
	
    // this is the crux of this problem!
    // convert dollar amount into cents, since otherwise our math below
    // will be unreliable because of the imprecision of floats.
    // we use round() because straight typecasting can convert some values
    // incorrectly, like 0.01 and 0.02
    int cents = round(dollars * CENTS_PER_DOLLAR);

    // we start out with zero coins used for change
    int coins = 0;
	
    // first, try using quarters (the largest amount)
    while (cents >= QUARTER)
    {
        // increment the coins counter by one
        coins++;

        // take a quarter away from the remaining change
        cents -= QUARTER;
    }
	
    // next, do the same thing for dimes
    while (cents >= DIME)
    {
        coins++;
        cents -= DIME;
    }
	
    // nickels next
    while (cents >= NICKEL)
    {
        coins++;
        cents -= NICKEL;
    }
	
    // lastly, use pennies until no more change is owed
    while (cents >= PENNY)
    {
        coins++;
        cents -= PENNY;
    }
	
    // print out the final answer
    printf("%d\n", coins);

    return 0;
}

