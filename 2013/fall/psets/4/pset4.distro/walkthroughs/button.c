/**
 * button.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Demonstrates a button.
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

    // instantiate button
    GButton button = newGButton("Button");
    setActionCommand(button, "click");

    // add button to southern region of window
    addToRegion(window, button, "SOUTH");

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

        // if action command is "click"
        if (strcmp(getActionCommand(event), "click") == 0)
        {
            printf("button was clicked\n");
        }
    }

    // that's all folks
    closeGWindow(window);
    return 0;
}
