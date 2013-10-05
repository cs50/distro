//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#define _XOPEN_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

// Stanford Portable Library
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"

// height and width of game's window in pixels
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 400

// height of a brick in pixels
#define BRICK_HEIGHT 10

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// gap between bricks in pixels
#define GAP 5

// height and width of paddle
#define PADDLE_WIDTH 60
#define PADDLE_HEIGHT 10

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// margin above bricks and below paddle
#define MARGIN 50

// how many milliseconds to wait after ball's moved before moving again
#define NAPTIME 10

// how many milliseconds to wait after ball's been lost
#define LIFETIME 500

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window);
GObject detectCollision(GWindow window, GObject ball);
GLabel initPoints(GWindow window);
void updatePoints(GWindow window, GLabel label, int points);

int main(int argc, char* argv[])
{
    // check for GOD mode
    bool god = false;
    if (argc == 2 && strcasecmp(argv[1], "GOD") == 0)
    {
        god = true;
    }

    // initialize window
    GWindow window = newGWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

    // initialize bricks
    initBricks(window);

    // initialize ball, centered in middle of window
    GOval ball = initBall(window);

    // initialize paddle, centered at bottom of window
    GRect paddle = initPaddle(window);

    // initialize points, centered in middle of window, just above ball
    GLabel label = initPoints(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;

    // seed pseudorandom number generator
    srand48(time(NULL));

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // wait for click to begin
        waitForClick();

        // decrement lives
        lives--;

        // generate random horizontal velocity for ball in [1.0, 2.0)
        double vx = drand48() + 1.0;

        // invert direction with 50/50 probability
        if (drand48() < 0.5)
        {
            vx = -vx;
        }

        // vertical velocity of ball
        double vy = 3.0;

        // while bricks remain and the ball's not missed the paddle
        while (getY(ball) < WINDOW_HEIGHT - 2 * RADIUS)
        {
            // check for mouse event
            GEvent e = getNextEvent(MOUSE_EVENT);

            // if we heard one
            if (god != true && e != NULL)
            {
                // if the event was movement
                if (getEventType(e) == MOUSE_MOVED)
                {
                    // keep paddle centered with cursor, unless at edge
                    double px = getX(e) - PADDLE_WIDTH / 2;
                    if (px < 0)
                    {
                        px = 0;
                    }
                    else if (px > WINDOW_WIDTH - PADDLE_WIDTH)
                    {
                        px = WINDOW_WIDTH - PADDLE_WIDTH;
                    }
                    setLocation(paddle, px, getY(paddle));
                }
            }

            // if we didn't
            else
            {
                // move ball
                move(ball, vx, vy);

                // if in GOD mode, keep paddle aligned with ball
                if (god == true)
                {
                    double x = getX(ball) + getWidth(ball) / 2 - getWidth(paddle) / 2;
                    setLocation(paddle, x, getY(paddle));
                }

                // if ball's at left or right edge, bounce
                if (getX(ball) < 0 || getX(ball) > WINDOW_WIDTH - 2 * RADIUS)
                {
                    vx = -vx;
                }

                // if ball's at top edge, bounce
                if (getY(ball) < 0)
                {
                    vy = -vy;
                }

                // check whether ball's collided with anything
                GObject object = detectCollision(window, ball);
                if (object != NULL)
                {
                    // if ball's collided with paddle, bounce
                    if (object == paddle)
                    {
                        vy = -vy;
                        setLocation(ball, getX(ball), getY(paddle) - 2 * RADIUS);
                    }

                    // if ball's collided with brick
                    else if (strcmp(getType(object), "GRect") == 0)
                    {
                        // remove brick from window
                        removeGWindow(window, object);

                        // decrement counter
                        bricks--;

                        // increment points
                        points++;

                        // update display of points
                        updatePoints(window, label, points);

                        // bounce
                        vy = -vy;
                    }
                }

                // slow down animation 
                pause(NAPTIME);
            }
        }

        // re-center ball
        if (lives > 0)
        {
            setLocation(ball, WINDOW_WIDTH / 2 - RADIUS, WINDOW_HEIGHT / 2 - RADIUS);
        }

        // pause before starting new life
        pause(LIFETIME);
    }

    // wait for click before exiting
    waitForClick();

    // game over
    closeGWindow(window);
    return 0;
}

/**
 * Initializes window with a grid of bricks.
 */
void initBricks(GWindow window)
{
    // determine width of each brick
    double width = (WINDOW_WIDTH - GAP * COLS) / COLS;

    // for each row
    for (int row = 0; row < ROWS; row++)
    {
        // determine vertical position of brick
        double y = MARGIN + row * (BRICK_HEIGHT + GAP);

        // for each column
        for (int col = 0; col < COLS; col++)
        {
            // determine horizontal position of brick
            double x = GAP / 2 + col * (width + GAP);

            // instantiate brick
            GRect brick = newGRect(x, y, width, BRICK_HEIGHT);

            // set brick's color
            switch (row)
            {
                // RED
                case 0:
                    setColor(brick, "RED");
                    break;

                // ORANGE
                case 1:
                    setColor(brick, "ORANGE");
                    break;

                // YELLOW 
                case 2:
                    setColor(brick, "YELLOW");
                    break;

                // GREEN
                case 3:
                    setColor(brick, "GREEN");
                    break;

                // CYAN
                case 4:
                    setColor(brick, "CYAN");
                    break;

                // GRAY
                default:
                    setColor(brick, "GRAY");
                    break;
            }

            // fill brick with color
            setFilled(brick, true);

            // add brick to window
            add(window, brick);
        }
    }
}

/**
 * Instantiates ball in center of window.  Returns ball.
 */
GOval initBall(GWindow window)
{
    double bx = WINDOW_WIDTH / 2 - RADIUS;
    double by = WINDOW_HEIGHT / 2 - RADIUS;
    GOval ball = newGOval(bx, by, 2 * RADIUS, 2 * RADIUS);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom-middle of window.
 */
GRect initPaddle(GWindow window)
{
    double x = (WINDOW_WIDTH - PADDLE_WIDTH) / 2;
    double y = WINDOW_HEIGHT - PADDLE_HEIGHT - MARGIN;
    GRect paddle = newGRect(x, y, PADDLE_WIDTH, PADDLE_HEIGHT);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);
    return paddle;
}

/**
 * Detects whether ball has collided with some object in window
 * by checking the four corners of its bounding box (which are
 * outside the ball's GOval, and so the ball can't collide with
 * itself).  Returns object if so, else NULL.
 */
GObject detectCollision(GWindow window, GObject ball)
{
    // ball's location
    double x = getX(ball);
    double y = getY(ball);

    // for checking for collisions
    GObject object;

    // check for collision at ball's top-left corner
    object = getGObjectAt(window, x, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's top-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-left corner
    object = getGObjectAt(window, x, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at ball's bottom-right corner
    object = getGObjectAt(window, x + 2 * RADIUS, y + 2 * RADIUS);
    if (object != NULL)
    {
        return object;
    }

    // no collision
    return NULL;
}

/**
 * Instantiates, configures, and returns label for points.
 */
GLabel initPoints(GWindow window)
{
    GLabel label = newGLabel("");
    setColor(label, "LIGHT_GRAY");
    setFont(label, "SansSerif-48");
    add(window, label);
    sendToBack(label);
    updatePoints(window, label, 0);
    return label;
}

/**
 * Updates points' label, keeping it centered in window.
 */
void updatePoints(GWindow window, GLabel label, int points)
{
    // update label
    char s[12];
    sprintf(s, "%i", points);
    setLabel(label, s);

    // center label
    double x = (getWidth(window) - getWidth(label)) / 2;
    double y = (getHeight(window) - getHeight(label)) / 2;
    setLocation(label, x, y);
}
