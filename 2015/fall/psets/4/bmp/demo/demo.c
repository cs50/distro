/**
 * slider.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Demonstrates a slider.
 */

// standard libraries
#define _SVID_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// Stanford Portable Library
#include "gevents.h"
#include "ginteractors.h"
#include "gobjects.h"
#include "gwindow.h"

// unistd.h
extern int unlink (const char *__name) __THROW __nonnull ((1));

bool display(GWindow window, char* bmp);

int main(int argc, char* argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: ./whodunit filename\n");
        return 1;
    }

    // filename must end in .bmp
    if (strlen(argv[1]) < 5 || strcmp(argv[1] + strlen(argv[1]) - 4, ".bmp") != 0)
    {
        printf("Invalid filename.\n");
        return 2;
    }

    // instantiate window
    GWindow window = newGWindow(640, 480);

    // instantiate slider
    addToRegion(window, newGLabel("0"), "SOUTH");
    GSlider slider = newGSlider(0, 255, 0);
    setActionCommand(slider, "slide");
    addToRegion(window, slider, "SOUTH");
    addToRegion(window, newGLabel("255"), "SOUTH");

    // display BMP
    if (!display(window, argv[1]))
    {
        return 3;
    }

    // listen for events
    while (true)
    {
        // wait for event
        GActionEvent event = waitForEvent(ACTION_EVENT);

        // if window was closed
        if (getEventType(event) == WINDOW_CLOSED)
        {
            break;
        }

        // if action command is "slide"
        if (strcmp(getActionCommand(event), "slide") == 0)
        {
            printf("slider was slid to %i\n", getValue(slider));
            display(window, argv[1]);
        }
    }

    // that's all folks
    closeGWindow(window);
    return 0;
}

bool display(GWindow window, char* bmp)
{
    // remembers whether window already has an image
    static GImage image = NULL;

    // if window currently has an image, remove it
    if (image != NULL)
    {
        removeGWindow(window, image);
        image = NULL;
    }

    // generate temporary filename for PNG
    char* tmp = tempnam(NULL, NULL);
    if (tmp == NULL)
    {
        printf("Could not generate temporary filename.\n");
        return false;
    }

    // allocate memory for command
    char* command = "convert bmp:%s png:%s";
    char* buffer = malloc(strlen(command) + strlen(bmp) + strlen(tmp) + 1);
    if (buffer == NULL)
    {
        printf("Could not allocate memory for command line.\n");
        return false;
    }

    // write command into buffer, substituting filenames for format codes
    sprintf(buffer, command, bmp, tmp);

    // convert BMP to PNG
    int code = system(buffer);

    // free buffer
    free(buffer);

    // if command didn't return (i.e., exit with) 0, conversion failed
    if (code != 0)
    {
        printf("Could not convert BMP to PNG.\n");
        return false;
    }

    // read PNG itself into memory
    image = newGImage(tmp);

    // delete temporary PNG from disk
    unlink(tmp);

    // free temporary filename's memory
    free(tmp);

    // if image's height or width is 0, something's wrong
    if (getHeight(image) == 0 && getWidth(image) == 0)
    {
        image = NULL;
        return false;
    }

    // add image to center of window
    int x = (getWidth(window) - getWidth(image)) / 2;
    int y = (getHeight(window) - getHeight(image)) / 2;
    addAt(window, image, x, y);

    // success
    return true;
}
