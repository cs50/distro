/**
 * text.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Demonstrates a text field.
 */

// standard libraries
#include <stdio.h>
#include <string.h>

// Stanford Portable Library
#include "gevents.h"
#include "ginteractors.h"
#include "gwindow.h"

int main(void)
{
    // instantiate window
    GWindow window = newGWindow(320, 240);

    // instantiate text field (wide enough for 10 characters)
    GTextField field = newGTextField(10);
    setActionCommand(field, "input");
    addToRegion(window, field, "SOUTH");

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

        // if action command is "input"
        if (strcmp(getActionCommand(event), "input") == 0)
        {
            printf("\"%s\" was inputted\n", getText(field));
        }
    }

    // that's all folks
    closeGWindow(window);
    return 0;
}
