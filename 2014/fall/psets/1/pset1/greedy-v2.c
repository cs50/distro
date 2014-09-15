/**
 * greedy_2.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Uses a greedy algorithm to determine how many coins are necessary to make
 * change for a user-provided dollar amount. Does some slick div/mod math to
 * make the problem super simple!
 *
 * Do note that because we're using GetFloat(), and floats are typically 32
 * bits (with 1 sign bit, 8 exponent bits, and 23 "mantissa," or "significand,"
 * bits), our program cannot precisely deal with change values greater than
 * ~2^16 = 65,536. At this point the gaps between floating point values are
 * 1/128, which is less than 1/100 (one penny). As we go higher, then the
 * gaps get smaller, so you'll notice that 131,072.01, for example, cannot be
 * represented accurately as a float.
 */

#include <cs50.h>
#include <math.h>
#include <stdio.h>

#define CENTS_PER_DOLLAR 100
#define QUARTER 25
#define DIME 10
#define NICKEL 5

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

    // with integer division, we know how many quarters can be used
    coins += cents / QUARTER;

    // we've only got the remainder left!
    cents = cents % QUARTER;

    // do the same for dimes
    coins += cents / DIME;
    cents = cents % DIME;

    // and likewise for nickels
    coins += cents / NICKEL;
    cents = cents % NICKEL;

    // no need to do anything fancy for pennies since x / 1 = x!
    coins += cents;

    // print out the final answer
    printf("%d\n", coins);

    return 0;
}

