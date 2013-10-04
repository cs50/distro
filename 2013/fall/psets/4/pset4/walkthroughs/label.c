//
// label.c
//
// David J. Malan
// malan@harvard.edu
//
// Draws a label in a window.
//

// standard libraries
#include <ctype.h>

// Stanford Portable Library
#include "gobjects.h"
#include "gwindow.h"

int main(void)
{
    // instantiate window
    GWindow window = newGWindow(320, 240);

    // instantiate label
    GLabel label = newGLabel("test\n");
    setFont(label, "SansSerif-36");
    add(window, label);

    //
    for (int i = 50; i >= 0; i--)
    {
        // to store 50 through 0 (with '\0'), we need <= 3 chars
        char s[3];

        // convert i from int to string
        sprintf(s, "%i", i);

        // update label
        setLabel(label, s);

        // center label
        double x = (getWidth(window) - getWidth(label)) / 2;
        double y = (getHeight(window) - getHeight(label)) / 2;
        setLocation(label, x, y);

        // linger for 500 milliseconds
        pause(100);
    }
 
    // that's all folks
    closeGWindow(window);
    return 0;
}
