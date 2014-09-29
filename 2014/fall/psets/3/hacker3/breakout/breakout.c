//
// breakout.c
//
// Computer Science 50
// Problem Set 3
//

// standard libraries
#define _XOPEN_SOURCE
#include <cs50.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>

// Stanford Portable Library
#include <spl/gevents.h>
#include <spl/gobjects.h>
#include <spl/gwindow.h>

// height and width of game's window in pixels
#define HEIGHT 600
#define WIDTH 400

// height of a brick in pixels
#define BRICK_HEIGHT 10

// number of rows of bricks
#define ROWS 5

// number of columns of bricks
#define COLS 10

// gap between bricks in pixels
#define GAP 5

// height and width of paddle
#define PADDLE_HEIGHT 10
#define PADDLE_WIDTH 70

// height and width of laser
#define LASER_WIDTH 10
#define LASER_HEIGHT 40

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

// special modes
#define SPEED_UP true
#define PADDLE_SHRINK true 
#define LASERS true
#define VARIABLE_SCORING true

// max speed prevents ball from teleporting over the paddle
#define MAX_SPEED 50

// by how much the speed increases (multiplication factor)
#define SPEED_INCREASE 1.1

// min paddle width prevents ball from going through paddle
#define PADDLE_MIN 20 

// by how much the paddle decreases (multiplication factor)
#define PADDLE_DECREASE 0.1

// prototypes
void initBricks(GWindow window);
GOval initBall(GWindow window);
GRect initPaddle(GWindow window, double x, double width);
GRect initLaser(GWindow window);
GObject detectCollision(GWindow window, GObject ball);
GObject detectLaserCollision(GWindow window, GObject laser);
GLabel initPoints(GWindow window);
void updatePoints(GWindow window, GLabel label, int points);
string getRandomColor();

int main(int argc, string argv[])
{
    // check for GOD mode
    bool god = false;
    if (argc == 2 && strcasecmp(argv[1], "GOD") == 0)
    {
        god = true;
    }

    // seed pseudorandom number generator
    srand48(time(NULL));

    // initialize window
    GWindow window = newGWindow(WIDTH, HEIGHT);

    // initialize bricks
    initBricks(window);

    // initialize ball, centered in middle of window
    GOval ball = initBall(window);

    // stores paddle width so the paddle can shrink if that mode is on
    double paddle_width = PADDLE_WIDTH;
    
    // initialize paddle, centered at bottom of window
    GRect paddle = initPaddle(window, WIDTH / 2 - paddle_width / 2, paddle_width);
    
    // initialize laser, drawn off-screen when not fired
    GRect laser = initLaser(window);

    // initialize points, centered in middle of window, just above ball
    GLabel label = initPoints(window);

    // number of bricks initially
    int bricks = COLS * ROWS;

    // number of lives initially
    int lives = LIVES;

    // number of points initially
    int points = 0;
    
    // used to help determine y-velocity influence based on paddle collision
    int edgeDelta;
    
    // used to store a temporary new X velocity before assigning to old
    int tempVelocityX;
    
    // used to store our ever-changing laser color
    // TODO: mention why 18 (or change to a constant if appropriate)
    char color[18];

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
        
        // vertical velocity of laser (initialized at 0 until fired)
        double laserVY = 0.0;
        
        // whether or not the laser is "active" (i.e., fired)
        bool laserActive = false;

        // while bricks remain and the ball's not missed the paddle
        while (bricks > 0 && getY(ball) < HEIGHT - 2 * RADIUS)
        {
            // if we're not playing in god mode...
            if (god != true)
            {
                // check for mouse event
                GEvent e = getNextEvent(MOUSE_EVENT + KEY_EVENT);
                if (e != NULL)
                {
                    // if the event was movement
                    if (getEventType(e) == MOUSE_MOVED)
                    {
                        // keep paddle centered with cursor, unless at edge
                        double px = getX(e) - paddle_width / 2;
                        if (px < 0)
                        {
                            px = 0;
                        }
                        else if (px > WIDTH - paddle_width)
                        {
                            px = WIDTH - paddle_width;
                        }
                        setLocation(paddle, px, getY(paddle));
                    }

                    // if the event was a click
                    else if (LASERS && getEventType(e) == MOUSE_CLICKED)
                    {
                        // only fire if laser isn't already active
                        if (!laserActive)
                        {
                            // flag laser as active
                            laserActive = true;

                            // TODO: explain
                            laserVY = -4.0;
                            
                            // set our location just above the center of the paddle
                            setLocation(laser, getX(paddle) + paddle_width / 2, 
                                HEIGHT - LASER_HEIGHT - MARGIN - PADDLE_HEIGHT - 5);

                            // change laser's color
                            setColor(laser, getRandomColor(color));
                        }
                    }
                }
            }
            else
            {
                // if in GOD mode, keep paddle aligned with ball
                double x = getX(ball) + getWidth(ball) / 2 - getWidth(paddle) / 2;
                
                // makes sure x is within window from the left...
                if (x < 0)
                {
                    x = 0;
                }

                // ... and from the right
                double max_x = WIDTH - paddle_width;
                if (x > max_x)
                {
                    x = max_x;
                }
                
                // updates paddle location
                setLocation(paddle, x, getY(paddle));
            }
            
            // update laser, if active
            if (laserActive)
            {
                // randomize laser's color
                setColor(laser, getRandomColor(color));

                // TODO: explain
                move(laser, 0, laserVY);
                
                // check for top-screen collision
                if (getY(laser) <= 0)
                {
                    // TODO: explain
                    laserActive = false;
                    setLocation(laser, -20, HEIGHT);
                }   
            }
            
            // now process update code...   
            // move ball
            move(ball, vx, vy);

            // if ball's at left or right edge, bounce
            if (getX(ball) < 0)
            {
                setLocation(ball, 0, getY(ball));
                vx = -vx;
            }
            if (getX(ball) > WIDTH - 2 * RADIUS)
            {
                setLocation(ball, WIDTH - 2 * RADIUS, getY(ball));
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
                // if ball's collided with paddle
                if (object == paddle)
                {
                    // bounce
                    vy = -vy;
                    
                    //
                    // modify angle of ball based on where it hits the paddle
                    //
                    
                    // first, determine if left or right of center for x flipping
                    if ((getX(ball) + RADIUS) < getX(paddle) + paddle_width / 2)
                    {
                        // get the distance between the paddle center and ball center
                        edgeDelta = (getX(paddle) + paddle_width / 2) - (getX(ball) + RADIUS);
                        
                        // the farther away, the steeper the angle
                        if (edgeDelta < 10)
                        {
                            tempVelocityX = 1;
                        }
                        else if (edgeDelta < 20) 
                        {
                            tempVelocityX = 2;
                        }
                        else if (edgeDelta < 30) 
                        {
                            tempVelocityX = 3;
                        }
                        else 
                        {
                            tempVelocityX = 4;
                        }
                        
                        // flip velocity if coming from left
                        if (vx > 0)
                        {
                            vx = -tempVelocityX;
                        }
                    }

                    // TODO: explain
                    else if ((getX(ball) + RADIUS) > getX(paddle) + paddle_width / 2)
                    {
                        // TODO: explain
                        edgeDelta = (getX(ball) + RADIUS) - (getX(paddle) + paddle_width / 2);
                        
                        // TODO: explain
                        if (edgeDelta < 10)
                        {
                            tempVelocityX = 1;
                        }
                        else if (edgeDelta < 20) 
                        {
                            tempVelocityX = 2;
                        }
                        else if (edgeDelta < 30) 
                        {
                            tempVelocityX = 3;
                        }
                        else 
                        {
                            tempVelocityX = 4;
                        }
                        
                        // TODO: explain
                        if (vx < 0)
                        {
                            vx = tempVelocityX;
                        }
                    }
                    
                    // TODO: explain
                    setLocation(ball, getX(ball), getY(paddle) - (2 * RADIUS));
                }

                // if ball has collided with laser
                else if (object == laser)
                {
                    // TODO: explain
                    setLocation(laser, -20, HEIGHT);
                    setLocation(ball, getX(ball), HEIGHT - 2 * RADIUS);
                    laserActive = false;
                }
                // if ball's collided with brick
                else if (strcmp(getType(object), "GRect") == 0)
                {
                    // remove brick from window
                    removeGWindow(window, object);

                    // decrement counter
                    bricks--;

                    // increment points
                    if (VARIABLE_SCORING)
                    {
                        points += (((BRICK_HEIGHT + GAP) * ROWS 
                            + MARGIN) - getY(object)) / GAP;
                    }
                    else
                    {
                        points++;
                    }

                    // update display of points
                    updatePoints(window, label, points);

                    // if the paddle powerup is on and its size is big enough, decrease it
                    if (PADDLE_SHRINK && paddle_width > PADDLE_MIN)
                    {
                        // shrinks paddle
                        paddle_width -= paddle_width * PADDLE_DECREASE;
                        
                        // reinitialises paddle in the same spot
                        int paddle_pos = getX(paddle);
                        removeGWindow(window, paddle);
                        paddle = initPaddle(window, paddle_pos, paddle_width);
                    }

                    // if the speed up powerup is on, speeds the ball up
                    if (SPEED_UP)
                    {
                        // modulo squared of the velocity vector
                        double speed_modulo = vx * vx + vy * vy;

                        // only increases speed if we haven't reached MAX
                        if (speed_modulo < MAX_SPEED)
                        {
                            vx *= SPEED_INCREASE;
                            vy *= SPEED_INCREASE;
                        }
                    }

                    // bounce
                    vy = -vy;
                }
            }
            
            // check whether laser has collided with any bricks
            object = detectLaserCollision(window, laser);
            if (object != NULL && object != laser)
            {
                if (strcmp(getType(object), "GRect") == 0)
                {
                    // remove brick from window
                    removeGWindow(window, object);

                    // decrement counter
                    bricks--;

                    // increment points
                    points++;

                    // update display of points
                    updatePoints(window, label, points);
                    
                    // update laser position to be back out of view
                    setLocation(laser, -20, HEIGHT);
                    
                    laserActive = false;
                }
            }

            // slow down animation 
            pause(NAPTIME);
        }

        // re-center ball
        if (lives > 0)
        {
            setLocation(ball, WIDTH / 2 - RADIUS, HEIGHT / 2 - RADIUS);
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
    double width = (WIDTH - GAP * COLS) / COLS;

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
    double bx = WIDTH / 2 - RADIUS;
    double by = HEIGHT / 2 - RADIUS;
    GOval ball = newGOval(bx, by, 2 * RADIUS, 2 * RADIUS);
    setColor(ball, "BLACK");
    setFilled(ball, true);
    add(window, ball);
    return ball;
}

/**
 * Instantiates paddle in bottom part of the window given horizontal position x and width.
 */
GRect initPaddle(GWindow window, double x, double width)
{
    double y = HEIGHT - PADDLE_HEIGHT - MARGIN;
    GRect paddle = newGRect(x, y, width, PADDLE_HEIGHT);
    setColor(paddle, "BLACK");
    setFilled(paddle, true);
    add(window, paddle);
    return paddle;
}


/**
 * Instantiates laser off-screen until fired during game loop.
 */
GRect initLaser(GWindow window)
{
    double x = -20;
    double y = HEIGHT - LASER_HEIGHT - MARGIN - PADDLE_HEIGHT;
    GRect laser = newGRect(x, y, LASER_WIDTH, LASER_HEIGHT);
    setColor(laser, "RED");
    setFilled(laser, true);
    add(window, laser);
    return laser;
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
 * Detects whether lasr has collided with some object in window
 * by checking the four corners of its bounding box.
 */
GObject detectLaserCollision(GWindow window, GObject laser)
{
    // ball's location
    double x = getX(laser);
    double y = getY(laser);

    // for checking for collisions
    GObject object;

    // check for collision at laser's top-left corner
    object = getGObjectAt(window, x, y-1);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at laser's top-right corner
    object = getGObjectAt(window, x + LASER_WIDTH, y-1);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at laser's bottom-left corner
    object = getGObjectAt(window, x, y + LASER_HEIGHT);
    if (object != NULL)
    {
        return object;
    }

    // check for collision at laser's bottom-right corner
    object = getGObjectAt(window, x + LASER_WIDTH, y + LASER_HEIGHT);
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

/**
 * Builds a random color from RGB constituents and passes it in
 * hex format via a char array that is returned.
 */
string getRandomColor(string buf)
{
    // generate a pseudorandom RGB triple
    unsigned red = rand() % 255;
    unsigned green = rand() % 255;
    unsigned blue = rand() % 255;
    
    // return hexadecimal color code
    sprintf(buf, "#%02x%02x%02x", red, green, blue);
    return buf;
}
