/**
 * binary_search.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Implements a binary search through a static, sorted array.
 * http://en.wikipedia.org/wiki/Binary_search
 */

#include <cs50.h>
#include <stdio.h>

#define SIZE 8

bool search(int needle, int haystack[], int size)
{
    int low = 0;
    int high = size - 1;
    
    while (low <= high)
    {
        // calculate the midpoint of the array
        // cast to unsigned int to avoid overflow if array is big enough
        // http://googleresearch.blogspot.com/2006/06/extra-extra-read-all-about-it-nearly.html
        int mid = ((unsigned int)low + (unsigned int)high) >> 1;

        // return if we've found a match!
        if (haystack[mid] == needle)
            return true;

        // gotta look in the upper half of the array
        else if (haystack[mid] < needle)
            low = mid + 1;

        // gotta look in the lower half of the array
        else
            high = mid - 1;

    }

    return false;
}

int main(void)
{
    int numbers[SIZE] = { 4, 8, 15, 16, 23, 42, 50, 108 };
    
    while (true)
    {
        printf("> ");
        int n = GetInt();
        if (n == -1)
            break;
        if (search(n, numbers, SIZE))
            printf("YES\n");
        else
            printf("NO\n");
    }
    return 0;
}
