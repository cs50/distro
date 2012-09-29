/**
 * selection_sort.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Implements selection sort on a static, unsorted integer array.
 * http://en.wikipedia.org/wiki/Selection_sort
 */

#include <stdio.h>

#define SIZE 8

void sort(int array[], int size)
{
    // sort ALL the numberz!
    for (int i = 0; i < size; i++)
    {
        int min = i;

        // find the index of the minimum value in the array
        for (int j = i; j < size; j++)
            if (array[j] < array[min])
                min = j;

        // if the index of the min value is not i, swap so that the min value
        // goes into the array at index i.
        if (min != i)
        {
            int temp = array[i];
            array[i] = array[min];
            array[min] = temp;
        }
    }
}

int main(void)
{
    int numbers[SIZE] = { 4, 15, 16, 50, 8, 23, 42, 108 };

    // print out the original array to prove it's unsorted!
    for (int i = 0; i < SIZE; i++)
        printf("%d ", numbers[i]);
    printf("\n");

    sort(numbers, SIZE);

    // print out the sorted array
    for (int i = 0; i < SIZE; i++)
        printf("%d ", numbers[i]);
    printf("\n");
    
    return 0;
}