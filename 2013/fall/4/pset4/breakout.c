//
// breakout.c
//
// Computer Science 50
// Problem Set 4
//

#include <stdio.h>

// TODO: are all necessary?
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

// radius of ball in pixels
#define RADIUS 10

// lives
#define LIVES 3

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// gap between blocks in pixels
#define GAP 5

// REMOVE?
/* Dimensions of the paddle */
#define PADDLE_WIDTH 60
#define PADDLE_HEIGHT 10

// margin above bricks and below paddle
#define MARGIN 50

// delay between turns in milliseconds
#define DELAY 500.0


/* Pause time in milliseconds */
#define SPEED 10.0

/* Function prototypes */

// prototypes
void init(GWindow gw);
GObject detect(GWindow gw, GObject ball);

int main(void)
{
    // initialize window
    GWindow window = newGWindow(WINDOW_WIDTH, WINDOW_HEIGHT);

    // initialize game
    init(window);

    // instantiate ball, centered in window
    double bx = WINDOW_WIDTH / 2 - RADIUS;
    double by = WINDOW_HEIGHT / 2 - RADIUS;
    GOval ball = newGOval(bx, by, 2 * RADIUS, 2 * RADIUS);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);

    // instantiate paddle, centered in window and bottom-aligned
    double px = (WINDOW_WIDTH - PADDLE_WIDTH) / 2;
    double py = WINDOW_HEIGHT - PADDLE_HEIGHT - MARGIN;
    GRect paddle = newGRect(px, py, PADDLE_WIDTH, PADDLE_HEIGHT);
    setColor(ball, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = 3;

    // keep playing until game over
    while (lives > 0 && bricks > 0)
    {
        // wait for click to begin
        waitForClick();

        // decrement lives
        lives--;

        // horizontal speed
        // TODO: use rand itself instead
        double vx = randomReal(1, 2);
        if (randomChance(0.5)) vx = -vx;

        // vertical speed
        double vy = 3.0;

        // while bricks remain and the ball's not below paddle
        while (bricks > 0 && getY(ball) < WINDOW_HEIGHT - RADIUS)
        {
            // listen for mouse event
            GEvent e = getNextEvent(MOUSE_EVENT);

            // if we hear one
            if (e != NULL)
            {
                // if the event was movement
                if (getEventType(e) == MOUSE_MOVED)
                {
                    // keep paddle centered with cursor, unless at edge
                    px = getX(e) - PADDLE_WIDTH / 2;
                    if (px < 0)
                    {
                        px = 0;
                    }
                    else if (px > WINDOW_WIDTH - PADDLE_WIDTH)
                    {
                        px = WINDOW_WIDTH - PADDLE_WIDTH;
                    }
                    setLocation(paddle, px, py);
                }
            }

            // if we don't
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
                GObject object = detect(window, ball);
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
                pause(SPEED);
            }
        }

        // pause before starting new life
        pause(DELAY);

        // re-center ball
        setLocation(ball, bx, by);
    }

    // wait for click to quit
    waitForClick();

    return 0; // TODO: REMOVE, WHY DOES CLANG WANT THIS FOR c99?
}

/**
 * Initializes the game with bricks.
 */
void init(GWindow window)
{
    double width = (WINDOW_WIDTH - GAP * COLS) / COLS;
    printf("%f\n", width);
    for (int row = 0; row < ROWS; row++)
    {
        double y = MARGIN + row * (BRICK_HEIGHT + GAP);
        for (int col = 0; col < COLS; col++)
        {
            double x = GAP / 2 + col * (width + GAP);

            printf("row %i, col %i is at x=%f, y = %f\n", row, col, x, y);

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
 * Detects whether ball has collided with some object in window.
 * Returns object if so, else NULL.
 */
GObject detect(GWindow window, GObject ball)
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
