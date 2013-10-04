//
// cursor.c
//
// David J. Malan
// malan@harvard.edu
//
// Draws a circle that follows user's cursor within a window.
//

// standard libraries
#include <stdio.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

int main(void)
{
    // instantiate window
    GWindow window = newGWindow(320, 240);

    // instantiate circle
    GOval circle = newGOval(0, 0, 50, 50);

    // add circle to window
    add(window, circle);
 
    // follow mouse forever
    while (true)
    {
        // check for mouse event
        GEvent event = getNextEvent(MOUSE_EVENT);

        // if we heard one
        if (event != NULL)
        {
            // if the event was movement
            if (getEventType(event) == MOUSE_MOVED)
            {
                // ensure circle follows top cursor
                double x = getX(event) - getWidth(circle) / 2;
                double y = getY(event) - getWidth(circle);
                setLocation(circle, x, y);
            }
        }
    }
}
