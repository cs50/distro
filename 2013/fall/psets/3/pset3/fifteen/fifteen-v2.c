/**
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Rob Bowden (rob@cs.harvard.edu)
 *
 * Implements the Game of Fifteen (generalized to d x d).
 *
 * Usage: ./fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [MIN,MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */

#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// board's minimal dimension
#define MIN 3

// board's maximal dimension
#define MAX 9

// represent the blank space in the board as 0
#define BLANK 0

// global variables that will keep track of the position
// of the blank on the board
int blank_i;
int blank_j;

// board, whereby board[i][j] represents row i and column j
int board[MAX][MAX];

// board's dimension
int d;

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);
void save(void);

int main(int argc, string argv[])
{
    // greet player
    greet();

    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: ./fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < MIN || d > MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            MIN, MIN, MAX, MAX);
        return 2;
    }

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // saves the current state of the board (for testing)
        save();

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = GetInt();

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep for animation's sake
        usleep(500000);
    }

    // that's all folks
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1,
 * (i.e., fills board with values but does not actually print them),
 * whereby board[i][j] represents row i and column j.
 */
void init(void)
{
    // the top left corner starts out one less than the dimension squared,
    // e.g. if d == 4, then the top left is 15
    int counter = d * d - 1;

    // iterate over the entire board
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            // insert into the current position on the board the value of our
            // counter and then decrement counter, so that the next position we
            // encounter (hah) will store one less than the current position,
            // and so on
            board[i][j] = counter--;
        }
    }

    // if the dimension is even, create a solvable board by swapping 1 and 2
    if (d % 2 == 0)
    {
        // d - 1 is the bottom row, and d - 2 is the second column from the
        // right (d - 3 is the third from the right)
        board[d - 1][d - 2] = 2;
        board[d - 1][d - 3] = 1;
    }

    // manually insert the blank into the bottom-right corner (not strictly
    // necessary as long as BLANK is #defined as 0, but if we change BLANK
    // above, the program should still work)
    board[d - 1][d - 1] = BLANK;

    // keep track of the position of our blank in global variables
    blank_i = d - 1;
    blank_j = d - 1;
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    // iterate over the board
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            // if the current position has a blank, print out _ instead of the
            // actual value stored in the board
            if (board[i][j] == BLANK)
            {
                printf(" _  ");
            }

            // else print out the value in the board
            else
            {
                // use %2d in order to print out single-digit numbers that take
                // up 2 spaces, so that our board doesn't look jagged when there
                // are both single- and double-digit integers
                printf("%2d  ", board[i][j]);
            }
        }
        // move to the next row
        printf("\n");
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false.
 */
bool move(int tile)
{
    // use these local variables to store the position of the tile if we
    // find it adjacent to the blank
    int tile_i, tile_j;

    // since we have the position of our blank tile in global variables
    // (blank_i and blank_j), we can see if the move is valid by looking
    // at the spaces adjacent to the blank to see if the tile is there

    // check if the tile is in the position above the blank.
    // notice that the "blank_i > 0" prevents us from looking above the blank if
    // the blank is in the top row of the board
    if (blank_i > 0 && board[blank_i - 1][blank_j] == tile)
    {
        // remember the position of the tile
        tile_i = blank_i - 1;
        tile_j = blank_j;
    }

    // check if the tile is in the position below the blank.
    // notice that the "blank_i < d - 1" prevents us from looking below the
    // blank if the blank is in the bottom row of the board
    else if (blank_i < d - 1 && board[blank_i + 1][blank_j] == tile)
    {
        tile_i = blank_i + 1;
        tile_j = blank_j;
    }

    // check if the tile is in the position to the left of the blank.
    // notice that the "blank_j > 0" prevents us from looking to the left of the
    // blank if the blank is in the left column of the board
    else if (blank_j > 0 && board[blank_i][blank_j - 1] == tile)
    {
        tile_i = blank_i;
        tile_j = blank_j - 1;
    }

    // check if the tile is in the position to the right of the blank.
    // notice that the "blank_j < d - 1" prevents us from looking to the right
    // of the blank if the blank is in the right column of the board
    else if (blank_j < d - 1 && board[blank_i][blank_j + 1] == tile)
    {
        tile_i = blank_i;
        tile_j = blank_j + 1;
    }

    // if the tile was not adjacent to the blank, the move was invalid
    else
    {
        return false;
    }

    // swap the values of the blank and the tile
    // note that we don't have to actually perform a swap (e.g., using
    // a temporary variable), since we already know the values of the
    // blank and of the tile
    board[tile_i][tile_j] = BLANK;
    board[blank_i][blank_j] = tile;

    // update our global variables that track the position of the blank, since
    // now the blank has moved to where the tile used to be
    blank_i = tile_i;
    blank_j = tile_j;

    // the move was successful!
    return true;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration),
 * else false.
 */
bool won(void)
{
    // in the winning configuration, the top left of the board is a 1
    int counter = 1;

    // iterate over the entire board, "counting" the tiles and making sure
    // that the current tile equals the current count
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            // we want to skip over the last position in the board,
            // since that is where the blank should be, and you shouldn't check
            // the value of the blank tile against counter
            if (i == d - 1 && j == d - 1)
            {
                continue;
            }

            // check to make sure that the current position of the board has the
            // value stored in the counter. if so, we increment counter so that
            // the next position of the board will be compared against
            // counter+1
            if (board[i][j] != counter++)
            {
                // if we encounter a tile that doesn't equal the current count,
                // then the tile is in the incorrect position, and we return
                // false to signify that the user has not won
                return false;
            }
        }
    }

    // if we managed to iterate over the entire board without encountering a
    // tile that was in the incorrect position, then the board is in the winning
    // configuration, and we can return true since the user has won
    return true;
}

/**
 * Saves the current state of the board to disk (for testing).
 */
void save(void)
{
    // log
    const string log = "log.txt";

    // delete existing log, if any, before first save
    static bool saved = false;
    if (!saved)
    {
        unlink(log);
        saved = true;
    }

    // open log
    FILE* p = fopen(log, "a");
    if (p == NULL)
    {
        return;
    }

    // log board
    fprintf(p, "{");
    for (int i = 0; i < d; i++)
    {
        fprintf(p, "{");
        for (int j = 0; j < d; j++)
        {
            fprintf(p, "%i", board[i][j]);
            if (j < d - 1)
            {
                fprintf(p, ",");
            }
        }
        fprintf(p, "}");
        if (i < d - 1)
        {
            fprintf(p, ",");
        }
    }
    fprintf(p, "}\n");

    // close log
    fclose(p);
}
