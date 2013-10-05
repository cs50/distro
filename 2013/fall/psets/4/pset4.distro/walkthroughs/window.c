//
// window.c
//
// David J. Malan
// malan@harvard.edu
//
// Draws a window.
//

// Stanford Portable Library
#include "gwindow.h"

int main(void)
{
    // instantiate window
    GWindow window = newGWindow(320, 240);
 
    // let's look at it for a while
    pause(5000);

    // that's all folks
    closeGWindow(window);
    return 0;
}
