/*************************************************************************
 * cat.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Mimics the `cat' command, sans stdin functionality.
 ************************************************************************/

#include <stdio.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Usage: cat file [file ...]\n");
        return 1;
    }

    // loop over all inputs, con*cat*enating them together!
    for (int i = 1; i < argc; i++)
    {
        // make sure to test that the file opens!
        FILE* file = fopen(argv[i], "r");
        if (file == NULL)
        {
            printf("cat: %s: No such file or directory\n", argv[i]);
            return 1;
        }
        
        // read the file one char (int?) at a time
        // fgetc returns an int, not a char, so for the c != EOF comparison
        // to work reliably, we need to use an int
        for (int c = fgetc(file); c != EOF; c = fgetc(file))
        {
            // print the char from the file to stdout
            // could also use printf("%c", c) or putc(c, stdout)
            putchar(c);
        }

        // gotta close this file before moving on!
        fclose(file);
    }

    return 0;
}
