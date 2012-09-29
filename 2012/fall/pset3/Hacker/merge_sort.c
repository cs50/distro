/**
 * merge_sort.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Implements recursive, top-down merge sort on a static, unsorted integer
 * array.
 * http://en.wikipedia.org/wiki/Merge_sort
 */

#include <stdio.h>

void sort(int array[], int size)
{
    // base case! array is guaranteed to be sorted
    if (size < 2)
        return;

    // split the list and create copies of each half
    int mid = size / 2;

    int left[mid];
    for (int i = 0; i < mid; i++)
        left[i] = array[i];

    // make sure you start this for loop at 0 too, not at mid!
    int right[size - mid];
    for (int i = 0; i < size - mid; i++)
        right[i] = array[mid + i];

    // recurrecurrecur...sionsionsion!
    sort(left, mid);
    sort(right, size - mid);

    // merge the lists back into array
    int left_index = 0, right_index = 0;
    for (int i = 0; i < size; i++)
    {
        // be careful to not index out of bounds!
        if (left_index < mid && right_index < size - mid)
        {
            if (left[left_index] <= right[right_index])
                array[i] = left[left_index++];
            else
                array[i] = right[right_index++];
        }
        else if (left_index >= mid)
        {
            array[i] = right[right_index++];
        }
        else
        {
            array[i] = left[left_index++];
        }
    }
}

int main(void)
{
    int numbers[] = { 4, 15, 16, 50, 8, 23, 42, 108 };

    // tricky tricky tricky!
    int size = sizeof(numbers) / sizeof(int);

    // print out the original array to prove it's unsorted!
    for (int i = 0; i < size; i++)
        printf("%d ", numbers[i]);
    printf("\n");

    // we have to pass the size in even with our trick above, since the trick
    // only works in the same scope in which the array was declared!
    sort(numbers, size);

    // print out the sorted array
    for (int i = 0; i < size; i++)
        printf("%d ", numbers[i]);
    printf("\n");
    
    return 0;
}
