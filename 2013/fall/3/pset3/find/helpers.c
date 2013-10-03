/**
 * helpers.c
 * 
 * Computer Science 50
 * Problem Set 3
 *
 * Rob Bowden (rob@cs.harvard.edu)
 *
 * Helper functions for Problem Set 3.
 */

#include <cs50.h>
#include <stdlib.h>

#include "helpers.h"

const int LIMIT = 65536;

/**
 * Returns true if value is in array of n values, else false.
 * Uses a recursive binary search algorithm.
 */
bool search(int value, int values[], int n)
{
    // if array has size 0, can't search
    if (n == 0)
    {
        return false;
    }
        
    // if the "middle" element in array equals value, return true
    int middle = n / 2;
    if (values[middle] == value)
    {
        return true;
    }
        
    // if the middle element was lower/higher than what we are looking for, 
    // we recursively call search on the upper/lower half (excluding the 
    // middle element) of the array with the size of array cut from middle 
    // over in the first case and set equal to middle in the latter
    else if (values[middle] < value)
    {
        return search(value, values + middle + 1, n - middle - 1);
    }
    else
    {
        return search(value, values, middle);
    }
}

/**
 * Sorts array of n values.  Returns true if successful, else false.
 * Implemented using selection sort.
 */
void sort(int values[], int n)
{
    // iterate over values, finding the i'th lowest # to go in the i'th index
    for (int i = 0; i < n - 1; i++)
    { 
        // at the beginning of each iteration, we know that the first (i-1)
        // elements are in their correct positions, so we search through the
        // rest of the array looking for the index of the lowest remaining #
        int min = i;
        for (int j = i; j < n - 1; j++)
        {
            if (values[j + 1] < values[min])
            {
                min = j + 1;
            }
        }
        
        // swap the minimum value into the i'th position
        int swaptemp = values[i];
        values[i] = values[min];
        values[min] = swaptemp;
    }
}
