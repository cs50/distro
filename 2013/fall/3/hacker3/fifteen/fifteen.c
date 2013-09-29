/**
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Rob Bowden (rob@cs.harvard.edu)
 *
 * Implements The Game of Fifteen (generalized to d x d).
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
#include <time.h>
#include <string.h>
#include <ctype.h>

// board's minimal dimension
#define MIN 3

// board's maximal dimension
#define MAX 9

// board, whereby board[i][j] represents row i and column j
int board[MAX][MAX];

// keeps track of where the empty space is
int col_0, row_0;

// dimensions
int d;

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);
void save(void);

int brute_force(int solution_path[],int steps);
void god_mode(void);

int main(int argc, char *argv[])
{
    // greet user
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
        printf("Board must be between %d x %d and %d x %d, inclusive.\n",
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
        string input = GetString();

        // if string not entered (user just hit CTRL-D), start loop over
        if (input==NULL)
        {
            continue;
        }

        // if the user requests GOD mode, call god_mode and end program after
        else if (!strcmp(input,"GOD"))
        {
            printf("You called? Here, let me help you.\n");
            god_mode();
            printf("Thank you for using GOD mode. Please come again!\n");
            return 0;
        }
        
        // move the entered tile number if possible, else report illegality
        if (!move(atoi(input)))
        {
            printf("\nIllegal move.\n");
            usleep(250000);
        }

        // sleep thread for animation's sake
        usleep(250000);
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
    // prints an ascii-art "Welcome to the game of fifteen"
    clear();
    printf(" _    _  ___  __    __  __  __  __  ___    ____  __     ____  _  _"
           "  ___ \n( \\/\\/ )(  _)(  )  / _)/  \\(  \\/  )(  _)  (_  _)/  \\ "
           "  (_  _)( )( )(  _)\n \\    /  ) _) )(__( (_( () ))    (  ) _)    "
           ")( ( () )    )(   )__(  ) _)\n  \\/\\/  (___)(____)\\__)\\__/(_/\\"
           "/\\_)(___)   (__) \\__/    (__) (_)(_)(___)\n  __   __   __  __  _"
           "__     __  ___    ___  __  ___  ____  ___  ___  _  _ \n / _) (  ) "
           "(  \\/  )(  _)   /  \\(  _)  (  _)(  )(  _)(_  _)(  _)(  _)( \\( )"
           "\n( (/\\ /__\\  )    (  ) _)  ( () )) _)   ) _) )(  ) _)  )(   ) _"
           ") ) _) )  ( \n \\__/(_)(_)(_/\\/\\_)(___)   \\__/(_)    (_)  (__)("
           "_)   (__) (___)(___)(_)\\_)\n\n"); 
    usleep(1000000);
}

/**
 * Checks to see if the pseudorandom board that init() creates is actually
 * winnable by summing how many times a smaller number come AFTER larger 
 * numbers on the board. If the sum is even, return winnable, else not.
 */
bool winnable_config(void)
{
    int count = 0;
    for (int i = 0; i < d * d - 1; i++)
    {
        for (int j = i + 1; j < d * d - 1; j++)
        {
            if (board[i / d][i % d] > board[j / d][j % d])
            {
                count++;
            }
        }
    }
    return !(count % 2);
}

/**
 * Initializes the game's board with tiles numbered 1 through d * d - 1,
 * (i.e., fills board with values but does not actually print them),
 * whereby board[i][j] represents row i and column j.  The tiles are
 * placed in a pseudorandom, yet solvable, configuration.
 */
void init(void)
{
    int tiles = d * d;
    
    // fill a template array with the numbers that will go on the board
    int numbers[tiles - 1];
    for (int i = 1; i < tiles; i++)
    {
        numbers[i - 1] = i;
    }

    // take a pseudrandom number out of the template array and put it on board
    srand(time(NULL));
    for (int i = 0, randnum; i < tiles - 1; i++)
    {
        randnum = rand() % (tiles - i - 1);
        board[i % d][i / d] = numbers[randnum];
        
        // take the number out of array so it can't be reused
        for (int j = randnum; j < tiles - 1 - i; j++)
        {
            numbers[j]=numbers[j + 1];
        }
    }
    
    // if the pseudorandom configuration is not solvable, swap the top-left two values
    if (!winnable_config())
    {
        int temp = board[0][0];
        board[0][0] = board[0][1];
        board[0][1] = temp;
    }
    
    // initialize the position of the blank space to the bottom right corner
    col_0 = d - 1;
    row_0 = d - 1;
    board[row_0][col_0] = 0;
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    // useful string formatting patterns, including ANSI escape sequences
    string centering = "                      ";
    string normal = "\033[0m";
    string blue = "\033[34;40m";
    string green = "\033[32m";
    string white_on_red = "\033[41;37m";
    
    // print the "Game of fifteen!" title bar
    printf("%s", centering);
    for(int i = 0; i < d / 2 - 1; i++)
    {
        printf("     ");
    }
    printf("%sGame of Fifteen!\n%s%s", white_on_red, blue, centering);
    
    // print the board itself
    for (int i = 0; i < d; i++)
    {
        // print the top border of a row
        for (int j = 0; j < d; j++)
        {
            printf("=====");
        }
        printf("=\n%s", centering);
        
        // print the left border of a box and the number it contains,
        // with a space for 0. Numbers in their correct location will be green.
        for (int j = 0; j < d; j++)
        {
            printf("%s| %s%2d%s ", blue,
                board[i][j]==i*d+j+1?green:normal, 
                board[i][j],board[i][j]==0?"\b\b  ":"");
        }
        
        // print the right side of last tile and center next line
        printf("%s|\n%s", blue, centering);
    }
    
    // print the bottom of the board and return escape sequences back to normal
    for (int i = 0; i < d; i++)
    {
        printf("=====");
    }
    printf("=%s\n", normal);
}


/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{ 
    // if we can move tile, the new position of tile will be the blank space
    int col = col_0, row = row_0;
    
    // look to the right of the blank space, if possible. If the tile is there,
    // the new position of the blank will be to the right.
    if (col_0 < d - 1 && board[row_0][col_0 + 1] == tile)
    {
        col_0++;
    }

    // look to the left of the blank space
    else if (col_0 > 0 && board[row_0][col_0 - 1] == tile)
    {
        col_0--;
    }

    // look below the blank space
    else if (row_0 < d - 1 && board[row_0 + 1][col_0] == tile)
    {
        row_0++;
    }

    // look above the blank space
    else if (row_0 > 0 && board[row_0 - 1][col_0] == tile)
    {
        row_0--;
    }

    // if the col and row #'s are still equal, the tile isn't next to the blank
    if (col == col_0 && row == row_0)
    {
        return false;
    }
    
    // swap the blank space and the desired tile
    board[row][col] = tile;
    board[row_0][col_0] = 0;
    return true;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    // checks to make sure each tile is in the right position
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            if (board[i][j] != d * i + j + 1 && !(i == d - 1 && j == d - 1))
            {
                return false;
            }
        }
    }
    return true;
}

/*************************************
 *************************************
 *                                   *
 *  GOD MODE functions start here!!  *
 *                                   *
 *************************************
 *************************************/

/**
 * A helper function to factor out code that recurs in the other slide
 * functions.
 */
void slide(int delta_col_0, int delta_row_0)
{
    // swap the values of the blank space and a neighboring tile
    board[row_0][col_0] = board[row_0 + delta_row_0][col_0 + delta_col_0];
    board[row_0 + delta_row_0][col_0 + delta_col_0]=0;
    
    // update the location of the blank space
    col_0 += delta_col_0;
    row_0 += delta_row_0;
    
    // god mode needs its own clear() and draw() since it does not get
    // cleared and drawn by the infinite loop in main.
    usleep(100000);
    clear();
    draw();
}

/**
 * Slides a tile DOWN into the blank space (the blank space moves up).
 */
void slide_down(void)
{
    slide(0, -1);
}

/**
 * Slides a tile UP into the blank space (the blank space moves down).
 */
void slide_up(void)
{
    slide(0, 1);
}

/**
 * Slides a tile LEFT into the blank space (the blank space moves right).
 */
void slide_left(void)
{
    slide(1, 0);
}

/**
 * Slides a tile RIGHT into the blank space (the blank space moves left).
 */
void slide_right(void)
{
    slide(-1, 0);
}

/**
 * Moves tile UP, with the blank beginning and ending to the right of tile
 */
void move_up(int* row)
{
    slide_down();
    slide_right();
    slide_up();
    slide_left();
    slide_down();
    (*row)--;
}

/**
 * Moves tile LEFT, with the blank beginning and ending to the right of tile
 */
void move_left(int* col)
{
    // if the tile is in the bottom row, we have to go ABOVE the tile in order
    // to move it left, else we can just go under it
    bool inverse = (row_0 == d - 1) ? true : false;
    (inverse) ? slide_down() : slide_up();
    slide_right();
    slide_right();
    (inverse) ? slide_up() : slide_down();
    slide_left();
    (*col)--;
}

/**
 * Moves tile RIGHT, with the blank beginning and ending to the right of tile
 */
void move_right(int* col)
{
    // if the tile is in the bottom row, we have to go ABOVE the tile in order
    // to move it right, else we can just go under it
    bool inverse = (row_0 == d - 1) ? true : false;
    slide_right();
    (inverse) ? slide_down() : slide_up();
    slide_left();
    slide_left();
    (inverse) ? slide_up() : slide_down();
    (*col)++;
}

/**
 * Moves tile DOWN, with the blank beginning and ending to the right of tile
 */
void move_down(int* row)
{
    slide_up();
    slide_right();
    slide_down();
    slide_left();
    slide_up();
    (*row)++;
}

/**
 * Moves tile diagonally up and to the left, with the blank beginning and 
 * ending to the right of tile.
 */
void move_up_left(int* row, int* col)
{
    slide_down();
    slide_right();
    slide_up();
    slide_right();
    slide_down();
    slide_left();
    (*row)--;
    (*col)--;
}

/**
 * Puts the tile to the left of the blank into correct row and col.
 */
void move_toward(int correct_row, int correct_col)
{
    // movement will be slightly different if we are filling in a row vs col
    bool inverse = (correct_row > correct_col) ? true : false;

    // when this function is called, we know that the blank tile is to
    // the right of our target tile
    int row = row_0;
    int col = col_0-1;
    
    // move tile as far up-left as possible
    while (row > correct_row && col > correct_col)
    {
        move_up_left(&row, &col);
    }
    
    // if filling in a row, then move the tile into the correct horizontal
    // position, and then up into the correct slot
    if (!inverse)
    {
        while (col > correct_col)
        {
            move_left(&col);
        }
        while (col < correct_col)
        {
            move_right(&col);
        }
        while (row > correct_row)
        {
            move_up(&row);
        }
    }

    // if filling in a col, then move the tile into the correct vertical
    // position, and then left into the correct slot
    else
    {
        while (row > correct_row)
        {
            move_up(&row);
        }
        while (row < correct_row)
        {
            move_down(&row);
        }
        while (col > correct_col)
        {
            move_left(&col);
        }
    }
}

/**
 * Searches for the tile in the board and stores its location in row and col
 */
void locate(int tile, int* row, int* col)
{
    for(int i = 0; i < d; i++)
    {
        for(int j = 0; j < d; j++)
        {
            if (board[i][j] == tile)
            {
                *row = i;
                *col = j;
                return;
            }
        }
    }
}

/**
 * Moves the blank so that it is to the right of the tile
 * that we are working on.
 */
void arrange_blank(int tile, int row, int col)
{
    // move the blank down to the correct row
    for(int i = row_0; i < row; i++)
    {
        // if the tile is just beneath the blank, then move around the tile
        // and the blank will be in the correct position
        if (board[row_0 + 1][col_0] == tile)
        {
            // if we are in the right-most column, then we have to slide
            // the tile to the left so the blank can be right of it
            if (col_0 == d - 1)
            {
                slide_right();
                slide_up();
                slide_left();
                return;
            }
            else
            {
                slide_left(); 
                slide_up(); 
                return;
            }
        }
        else
        {
            slide_up();
        }
    }
    for (int i = row_0; i > row; i--)
    {
        // if the tile is just above the blank, then move around the tile
        // and the blank will be in the correct position
        if (board[row_0 - 1][col_0] == tile)
        {
            // if we are in the right-most column, then we have to slide
            // the tile to the left so the blank can be right of it
            if (col_0 == d - 1)
            {
                slide_right();
                slide_down();
                slide_left();
                return;
            }
            else
            {
                slide_left();
                slide_down();
                return;
            }
        }
        else
        {
            slide_down();
        }
    }

    // move the blank left to the correct row
    for (int i = col_0; i > col; i--)
    {
        if (board[row_0][col_0 - 1] == tile)
        {
            return;
        }
        else
        {
            slide_right();
        }
    }
            
    // move the blank right to the correct row
    for (int i = col_0; i < col; i++)
    {
        if (board[row_0][col_0 - 1] == tile)
        {
            return;
        }
        else
        {
            slide_left();
        }
    }
}

/**
 * A helper function to factor out common code in brute_force.
 */
int brute_helper(int tile, int solution_path[], int steps)
{
    // try moving the current tile and store it in solution_path
    solution_path[steps] = tile;
    move(tile);
    
    // if that move put board in a winning config, return #  steps to that move
    if (won())
    {
        return steps + 1;
    }
        
    // if we haven't one yet, we see if moves AFTER this move put the board in
    // a winning config, and if so return the total steps to the final move
    int total_steps;
    if ((total_steps = brute_force(solution_path, steps + 1)))
    {
        return total_steps;
    }
        
    // if we get to this point, then this move did not lead us in the direction
    // of winning, so we move the tile back to where it was and return that
    // the move was unsuccesful
    move(tile);
    return 0;
}

/**
 * Applies bruce force.
 */
int brute_force(int solution_path[32], int steps)
{
    int total_steps;
    
    // if we've reached 32 steps, NO 3x3 board should take that long to solve,
    // so return that we have to backtrack
    if (steps == 32)
    {
        return 0;
    }
        
    // tries to see if moving a tile up, if possible, will lead 
    // to a winning configuration
    if (row_0 > d - 3)
    {
        // checks to make sure we aren't backtracking by moving 
        // a piece back and forth
        if (!steps || board[row_0 - 1][col_0] != solution_path[steps - 1])
        {
            // if brute_helper returns nonzero, then winning path found
            if ((total_steps = brute_helper(board[row_0 - 1][col_0], solution_path, steps)))
            {
                return total_steps; 
            }
        }
    }
            
    // tries to see if moving a tile down, if possible, will lead 
    // to a winning configuration 
    if (row_0 < d - 1)
    {
        if (!steps || board[row_0 + 1][col_0] != solution_path[steps - 1])
        {
            if ((total_steps = brute_helper(board[row_0 + 1][col_0], solution_path, steps)))
            {
                return total_steps;
            }
        }
    }
               
    // tries to see if moving a tile right, if possible, will lead 
    // to a winning configuration
    if (col_0 < d - 1)
    {
        if (!steps || board[row_0][col_0 + 1] != solution_path[steps - 1])
        {
            if ((total_steps = brute_helper(board[row_0][col_0 + 1], solution_path, steps)))
            {
                return total_steps;
            }
        }
    }

    // tries to see if moving a tile left, if possible, will lead 
    // to a winning configuration
    if (col_0 > d - 3)
    {
        if (!steps || board[row_0][col_0 - 1] != solution_path[steps - 1])
        {
            if ((total_steps = brute_helper(board[row_0][col_0 - 1], solution_path, steps)))
            {
                return total_steps;
            }
        }
    }
               
    // if none of the above have lead to a winning configuration, then return
    // 0 since the move that led us here was futile so we have to backtrack
    return 0;
}

/**
 * Function that will ultimately solve the board. It works by
 * filling in the top most unsorted row and left most unsorted column 
 * until we are left with a 3x3 box in the bottom right corner, at which point
 * it tries to find a path to the winning puzzle by brute force.
 */
void god_mode(void)
{
    // This outermost for loop keeps track of the corner around which we are 
    // filling in first the row, and then the column. This continues until
    // we are left with a 3x3 unsolved box in the bottom-right corner.
    for (int corner = 0, tile, row, col, correct_row, correct_col; corner < d - 3; corner++)
    { 
        // fill in the top-most remaining row from left to right
        for (int j = corner; j < d; j++)
        {
            // calculate the tile that we want to move to this position
            tile = corner * d + j + 1;
            
            // store the correct coordinates of the tile in new variables,
            // since we may have to change where we want the tile to go
            correct_row = corner;
            correct_col = j;
            locate(tile, &row, &col);
            
            // If a tile is already in its appropriate position, then we can
            // continue to the next tile.
            if (row == correct_row && col == correct_col)
            {
                // Make sure we don't continue unless both 2nd-to-last and 
                // last tiles in row are correct since those are special cases
                if (correct_col != d - 2 || board[row][col + 1] == tile + 1)
                {
                    continue;
                }
            }
                    
            // If we are focused on the last tile in a row, then we want to
            // shift where it should go down and to the left
            if (correct_col == d - 1)
            {
                correct_row++;
                correct_col--;
            }
            
            // arrange the blank space so that it is to the right of the tile
            arrange_blank(tile,row,col);
            
            // move the tile to its correct position            
            move_toward(correct_row, correct_col);
            
            // If the tile is the 2nd-to-last # in the row, then it must 
            // be slided right so we can put the last number into place
            if (correct_col == d - 2)
            {
                // If we encounter the last # in the row while trying to
                // move the 2nd-to-last #, we can't put the last tile to the
                // left of the 2nd-to-last, or we'll be stuck
                if (board[row_0 + 1][col_0 - 1] == tile + 1)
                {
                    slide_up();
                    slide_right();
                    slide_up();
                    slide_left();
                    slide_down();
                    slide_down();
                }
                slide_right();
            }
            // If we are on the last # in row, we need to do these slides
            // to place both the 2nd-to-last and last tiles correctly
            if (!(tile % d))
            {
                slide_down();
                slide_left();
                slide_up();
            }
        }
        
        // move the blank space down so the next arrange_blank does not 
        // screw up a correct number in the next column to be arranged
        while (row_0 < d - 1)
        {
            slide_up();
        }
        
        
        // fill in the left-most remaining column from top to bottom. steps are
        // similar to the above, except changed for filling in columns
        for (int j = corner + 1; j < d; j++)
        {
            tile = j * d + corner + 1;
            correct_col = corner;
            correct_row = j;
            locate(tile, &row, &col);
            
            // if the number is already in the correct place, skip to next tile
            if (row == correct_row && col == correct_col)
            {
                if (j != d - 2 || board[row + 1][col] == tile + d)
                {
                    continue;
                }
            }
            
            // if last # in row, change to 1 col over from correct destination
            if (j == d - 1)
            {
                correct_col++;
            }
            
            arrange_blank(tile,row,col);
            move_toward(correct_row, correct_col);
            
            // if 2nd-to-last number, arrange it so we can fit in last number
            if (j == d - 2)
            {
                if (board[row_0 + 1][col_0] == tile + d)
                {
                    slide_left();
                    slide_up();
                    slide_right();
                    slide_right();
                    slide_down();
                    slide_left();
                }
                else
                {
                    slide_up();
                    slide_right();
                    slide_down();
                    slide_left();
                }
            }
            
            // if last number, do slides to put 2nd-to-last and last in place
            if (j == d - 1)
            {
                slide_down();
                slide_right();
                slide_right();
                slide_up();
                slide_left();
            }
        }
        // The column is complete!
        
        // Now we want to move the blank all the way to the right so that
        // arrange_blank doesn't mess up any already-correct tiles
        // in the next row
        while (col_0 < d - 1)
        {
            slide_left();
        }
    }
    // At this point, the whole board has been solved except for the final
    // 3x3 bottom-right corner box
    
    // Save the current state of 3x3 box since brute force moves tiles around
    int temp_board[3][3];
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            temp_board[i][j] = board[i + d - 3][j + d - 3];
        }
    }
    int old_row_0 = row_0;
    int old_col_0 = col_0;
    
    // The highest number of moves necessary to complete a 3x3 board from
    // any configuration is 31 steps, so we will limit the size of the solution
    // path to 32. brute_force stores the moves in solution_path and
    // returns the number of steps through solution_path we have to make
    int solution_path[32];
    int steps = brute_force(solution_path, 0);
    
    // Restore the state of the board from before brute_force was called
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            board[i + d - 3][j + d - 3] = temp_board[i][j];
        }
    }
    row_0 = old_row_0;
    col_0 = old_col_0;
    
    // make the moves that are stored in solution_path and redraw board
    for (int i = 0; i < steps; i++)
    {
        move(solution_path[i]);
        usleep(100000);
        clear();
        draw();
    }
    // The puzzle is solved!
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
