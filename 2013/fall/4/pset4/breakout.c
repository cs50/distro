//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

// standard libraries
#include <stdio.h>

// Stanford Portable Library
#include "cslib.h"
#include "gevents.h"
#include "gobjects.h"
#include "gwindow.h"
#include "random.h"

// height and width of game's window in pixels
#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 400

// height of a brick in pixels
#define BRICK_HEIGHT 10

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// gap between blocks in pixels
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

// how many milliseconds to wait between checks for mouse events
#define NAPTIME 10

// how many milliseconds to wait after ball's been lost
#define LIFETIME 500

// prototypes
void initBricks(GWindow gw);
GOval initBall(GWindow gw);
GRect initPaddle(GWindow gw);
GObject detectCollision(GWindow gw, GObject ball);

int main(void)
{
    // initialize window
    GWindow window = newGWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

    // initialize bricks
    initBricks(window);

    // initialize ball, centered in middle of window
    GOval ball = initBall(window);

    // initialize ball, centered at bottom of window
    GRect paddle = initPaddle(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // wait for click to begin
        waitForClick();

        // decrement lives
        lives--;

        // horizontal velocity of ball
        double vx = randomReal(1.0, 2.0);
        if (randomChance(0.5))
        {
            vx = -vx;
        }

        // vertical velocity of ball
        double vy = 3.0;

        // while bricks remain and the ball's not below paddle
        while (bricks > 0 && getY(ball) < WINDOW_HEIGHT - RADIUS)
        {
            // check for mouse event
            GEvent e = getNextEvent(MOUSE_EVENT);

            // if we heard one
            if (e != NULL)
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
                    else
                    {
                        // remove brick from window
                        removeGWindow(window, object);

                        // decrement counter
                        bricks--;

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
 * Initializes the game with bricks.
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
    double px = (WINDOW_WIDTH - PADDLE_WIDTH) / 2;
    double py = WINDOW_HEIGHT - PADDLE_HEIGHT - MARGIN;
    GRect paddle = newGRect(px, py, PADDLE_WIDTH, PADDLE_HEIGHT);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);
    return paddle;
}

/**
 * Detects whether ball has collided with some object in window.
 * Returns object if so, else NULL.
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
