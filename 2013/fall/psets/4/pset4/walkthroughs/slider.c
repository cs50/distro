/**
 * slider.c
 *
 * David J. Malan
 * malan@harvard.edu
 *
 * Demonstrates a slider.
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

    // instantiate slider
    addToRegion(window, newGLabel("0"), "SOUTH");
    GSlider slider = newGSlider(0, 100, 50);
    setActionCommand(slider, "slide");
    addToRegion(window, slider, "SOUTH");
    addToRegion(window, newGLabel("100"), "SOUTH");

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
        }
    }

    // that's all folks
    closeGWindow(window);
    return 0;
}
