/*************************************************************************
 * hacker_typer.c
 *
 * Nate Hardison <nate@cs.harvard.edu>
 *
 * Amaze your friends! Impress even your enemies! http://hackertyper.net/
 ************************************************************************/

#include <stdio.h>
#include <termios.h>

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Usage: hacker_typer file\n");
        return 1;
    }

    // clear the screen
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);

    // open up the file to type
    FILE* file = fopen(argv[1], "r");
    if (file == NULL)
    {
        printf("hacker_typer: %s: No such file or directory\n", argv[1]);
        return 1;
    }

    // turn off keystroke echoes and "canonical settings"
    struct termios normal_settings, hacker_typer_settings;
    tcgetattr(0, &normal_settings);
    hacker_typer_settings = normal_settings;
    hacker_typer_settings.c_lflag &= ~ECHO;
    hacker_typer_settings.c_lflag &= ~ICANON;
    hacker_typer_settings.c_cc[VTIME] = 0;
    hacker_typer_settings.c_cc[VMIN] = 1;
    tcsetattr(0, TCSANOW, &hacker_typer_settings);

    // read in the file one char at a time
    // fgetc returns an int, not a char, so we need to use an int for the
    // c != EOF comparison to be reliable!
    for (int c = fgetc(file); c != EOF; c = fgetc(file))
    {
        // pull a keystroke from stdin and throw it away 
        fgetc(stdin);

        // print the next char from the file to stdout
        // could also use printf("%c", c) or putc(c, stdout)
        putchar(c);
    }

    // restore our normal terminal settings
    tcsetattr(0, TCSANOW, &normal_settings);

    // since the loop ended, we know fgetc returned EOF
    // this could mean either a true end-of-file or an error
    // test for the error!
    if (ferror(file))
    {
        // clean up and notify user
        fclose(file);
        printf("hacker_typer: %s: Error reading file\n", argv[1]);
        return 1;
    }

    fclose(file);

    return 0;
}
