//
// click.c
//
// David J. Malan
// malan@harvard.edu
//
// Prints cursor's location when mouse's button is clicked.
//

// standard libraries
#include <stdio.h>

// Stanford Portable Library
#include "gevents.h"
#include "gwindow.h"

int main(void)
{
    // instantiate window
    GWindow window = newGWindow(320, 240);

    // listen forever
    while (true)
    {
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if we heard one
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_CLICKED)
            {
                // print click's coordinates
                printf("%.0f,%.0f\n", getX(event), getY(event));
            }
        }
    }
}
