/*************************************************************************
 * cp.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Mimics the `cp' command
 ************************************************************************/

#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Usage: cp source dest\n");
        return 1;
    }

    // try opening up the source file
    FILE* source = fopen(argv[1], "r");
    if (source == NULL)
    {
        printf("cp: %s: No such file\n", argv[1]);
        return 1;
    }

    // now open up the destination file
    // use w+ to create the file (assuming it doesn't exist)
    FILE* dest = fopen(argv[2], "w+");
    if (dest == NULL)
    {
        // make sure to close the source file before exiting!
        fclose(source);
        printf("cp: %s: Cannot open file for writing\n", argv[2]);
        return 1;
    }

    // read the contents of the source file one char (int!) at a time
    // remember to use int so that the c != EOF comparison is reliable
    for (int c = fgetc(source); c != EOF; c = fgetc(source))
    {
        // write the character to the destination file!
        // could also use fprintf(dest, "%c", c)
        putc(c, dest);
    }

    // you leak memory if you forget to close!
    fclose(source);
    fclose(dest);

    return 0;
}
