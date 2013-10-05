/**
 * checkbox.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Demonstrates a checkbox.
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

    // instantiate checkbox
    GCheckBox checkbox = newGCheckBox("I agree");
    setActionCommand(checkbox, "check");
    addToRegion(window, checkbox, "SOUTH");

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

        // if action command is "check"
        if (strcmp(getActionCommand(event), "check") == 0)
        {
            if (isSelected(checkbox))
            {
                printf("checkbox was checked\n");
            }
            else
            {
                printf("checkbox was unchecked\n");
            }
        }
    }

    // that's all folks
    closeGWindow(window);
    return 0;
}
