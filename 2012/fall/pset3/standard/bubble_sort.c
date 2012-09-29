/**
 * bubble_sort.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Implements bubble sort on a static, unsorted integer array.
 * http://en.wikipedia.org/wiki/Bubble_sort
 */

#include <stdio.h>

#define SIZE 8

void sort(int array[], int size)
{
    // sort ALL the numberz!
    for (int i = 0; i < size; i++)
    {
        // as the big values "bubble" to the end of the array, we only have to
        // focus on the first size - i values (-1 to accommodate array[j + 1])
        for (int j = 0; j < size - i - 1; j++)
        {
            // swap if the next element in the array is smaller
            if (array[j] > array[j + 1])
            {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
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