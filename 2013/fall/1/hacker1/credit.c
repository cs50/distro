/**
 * credit.c
 *
 * Batool Ali
 * David J. Malan
 *
 * Takes a credit card number and determines
 * whether it is valid and, if it is valid,
 * its type.
 */

#include <cs50.h>
#include <stdio.h>

// constant credit card company identifiers and number lengths
#define AMEX_1 34
#define AMEX_2 37
#define AMEX_LENGTH 15

#define FIRST_MC 51
#define LAST_MC 55
#define MC_LENGTH 16

#define FIRST_VISA 40
#define LAST_VISA 49
#define VISA_LENGTH1 13
#define VISA_LENGTH2 16


int main(void)
{
    // request number
    printf("Number: ");
    long long cc_no = GetLongLong();

    // prepare for some math
    int checksum = 0;
    int first_two_digits = 0;
    bool mult_by_two = false;
    int cc_no_length = 0;

    // iterate over digits
    while (cc_no > 0)
    {
        // for each digit, starting from the last:
        int cur_digit = cc_no % 10;

        // add the new digit to the checksum, doubling if necessary
        if (mult_by_two)
        {
            // accounting for the case where need to add digits together
            checksum += (cur_digit < 5) ? (cur_digit * 2) : (cur_digit * 2 - 9);
        }
        else
        {
            checksum += cur_digit;
        }

        // keep track of beginning of number to identify credit card company
        first_two_digits = cur_digit * 10 + (first_two_digits / 10);

        // update state and drop last digit
        mult_by_two = !mult_by_two;
        cc_no = cc_no / 10;
        cc_no_length++;
    }

    // validate checksum, company identifier, and number length
    if (checksum % 10)
    {
        printf("INVALID\n");
    }
    else if (((first_two_digits == AMEX_1) ||
        (first_two_digits == AMEX_2)) &&
        (cc_no_length == AMEX_LENGTH))
    {
        printf("AMEX\n");
    }
    else if ((first_two_digits >= FIRST_VISA) &&
        (first_two_digits <= LAST_VISA) &&
        ((cc_no_length == VISA_LENGTH1) ||
        (cc_no_length == VISA_LENGTH2)))
    {
        printf("VISA\n");
    }
    else if ((first_two_digits >= FIRST_MC) &&
        (first_two_digits <= LAST_MC) &&
        (cc_no_length == MC_LENGTH))
    {
        printf("MASTERCARD\n");
    }
    else
    {
        printf("INVALID\n");
    }
}
