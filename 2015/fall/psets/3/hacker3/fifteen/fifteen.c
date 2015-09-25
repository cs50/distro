/**
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

int e_i, e_j;

// prototypes
void clear();
void greet();
void init();
void draw();
bool move();
bool won(int);
void god_solve(int);

int main(int argc, string argv[])
{
    // seed PRNG
    srand48(time(NULL));

    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: %s d\n", argv[0]);
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // check for win
        if (won(0))
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        string dir = GetString();
        if (dir == NULL)
        {
            continue;
        }
        int tile;
        if (strcmp(dir, "GOD") == 0)
        {
            printf("entering GOD mode...\n");
            god_solve(d);
            return 0;
        }
        else
        {
            tile = atoi(dir);
        }

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear()
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet()
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Sorts numbers.
 */
void q_sort(double numbers[], int sub[], int left, int right)
{
    double pivot;
    int l_hold, r_hold;
    int subpivot;

    l_hold = left;
    r_hold = right;

    pivot = numbers[left];
    subpivot = sub[left];

    while (left < right)
    {
        while ((numbers[right] >= pivot) && (left < right))
        {
            right--;
        }
        if (left != right)
        {
            numbers[left] = numbers[right];
            sub[left] = sub[right];
            left++;
        }
        while ((numbers[left] <= pivot) && (left < right))
        {
            left++;
        }
        if (left != right)
        {
            numbers[right] = numbers[left];
            sub[right] = sub[left];
            right--;
        }
    }
    numbers[left] = pivot;
    sub[left] = subpivot;

    pivot = left;

    left = l_hold;
    right = r_hold;

    if (left < pivot)
    {
        q_sort(numbers, sub, left, pivot - 1);
    }
    if (right > pivot)
    {
        q_sort(numbers, sub, pivot + 1, right);
    }
}
void quickSort(double numbers[], int sub[], int array_size)
{
    q_sort(numbers, sub, 0, array_size - 1);
}

/**
 * Returns 1 if solvable, else 0.
 */
int solvable(int tiles[])
{
    int inv = 0;
    int i, j;
    for (i = 0; i < d * d - 1; i++)
    {
        for (j = i + 1; j < d * d - 1; j++)
        {
            if (tiles[i] > tiles[j])
            {
                inv++;
            }
        }
    }

    // note: since we always place empty tile on the very last row
    // we want the #inv always to be even
    return (inv % 2 == 0);
}

/**
 * Initializes the game's board with tiles (numbered 1 through d*d - 1),
 * i.e., fills 2D array with values but does not actually print them).  
 */
void init()
{
    int i, j;
    double rvalues[d * d - 1];
    int tvalues[d * d];
    do
    {
        for(i = 0; i < d * d - 1; i++)
        {
            rvalues[i] = drand48();
            tvalues[i] = i + 1;
        }
        quickSort(rvalues, tvalues, d * d - 1);
    }
    while(!solvable(tvalues));
    tvalues[d * d - 1] = 0;
    for(i = 0; i < d; i++)
    {
        for(j = 0; j < d; j++)
        {
            board[i][j] = tvalues[i * d + j];
        }
    }
    e_i = d - 1;
    e_j = d - 1;
}

/**
 * Prints the board in its current state.
 */
void draw()
{
    int i, j;
    printf("\033[31m");
    for(j = 0; j < d; j++)
    {
        printf("-----");
    }
    printf("-\033[0m\n");
    for (i = 0; i < d; i++)
    {
        for (j = 0; j < d; j++)
        {
            if (board[i][j] != 0)
            {
                if (board[i][j] == i * d + j + 1)
                {
                    printf("\033[31m|\033[32m %-2d \033[0m", board[i][j]);
                }
                else
                {
                    printf("\033[31m|\033[0m %-2d ", board[i][j]);
                }
            }
            else
            {
                printf("\033[31m|\033[0m    ");
            }
        }
        printf("\033[31m|\033[0m\n");
        printf("\033[31m");
        for(j = 0; j < d; j++)
        {
            printf("-----");
        }
        printf("-\033[0m\n");
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{
    // look up
    if (e_i != 0)
    {
        if (tile == board[e_i - 1][e_j])
        {
            board[e_i][e_j] = tile;
            e_i = e_i - 1;
            board[e_i][e_j] = 0;
            return true;
        }
    }

    // look down
    if (e_i != d - 1)
    {
        if (tile == board[e_i + 1][e_j])
        {
            board[e_i][e_j] = tile;
            e_i = e_i + 1;
            board[e_i][e_j] = 0;
            return true;
        }
    }

    // look left
    if (e_j != 0)
    {
        if (tile == board[e_i][e_j - 1])
        {
            board[e_i][e_j] = tile;
            e_j = e_j - 1;
            board[e_i][e_j] = 0;
            return true;
        }
    }

    // look right
    if (e_j != d - 1)
    {
        if (tile == board[e_i][e_j + 1])
        {
            board[e_i][e_j] = tile;
            e_j = e_j + 1;
            board[e_i][e_j] = 0;
            return true;
        }
    }

    return false;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(int off)
{
    int ii;
    int i, j;
    int prev = -99999;
    int d2 = d - off;
    for (ii = 0; ii < d2 * d2 - 1; ii++)
    {
        i = ii / d2 + off;
        j = ii % d2 + off;

        if (board[i][j] == 0 || board[i][j] < prev)
        {
            return false;
        }

        prev = board[i][j];
    }

    return true;
}

/**
 * s = number of steps already taken;
 * next would be step s+1 to be stored in solutions[s]
 */
int brute(int offset, int s, int solutions[32])
{
    // we've reached limit of DFS, return
    if (s == 32)
    {
        return 0;
    }

    int t;

    int total_s;

    // try up
    if (e_i > offset)
    {
        t = board[e_i - 1][e_j];

        if (!(s > 0 && t == solutions[s - 1]))
        {
            // make a move
            solutions[s] = t;
            move(t);

            if (won(offset))
            {
                return s + 1;
            }

            // if won after this move

            if ((total_s = brute(offset, s + 1, solutions)))
            {
                return total_s;
            }

            // if neither, then moving up is bad. we need to undo this
            move(t);
        }
    }

    // try down
    if (e_i != d - 1)
    {
        t = board[e_i + 1][e_j];

        if (!(s > 0 && t == solutions[s - 1]))
        {
            // make a move
            solutions[s] = t;
            move(t);

            if (won(offset))
            {
                return s + 1;
            }

            // if won after this move
            if ((total_s = brute(offset, s + 1, solutions)))
            {
                return total_s;
            }

            // if neither, then moving down is bad. we need to undo this
            move(t);
        }   
    }

    // try right
    if (e_j != d - 1)
    {
        t = board[e_i][e_j + 1];

        if (!(s > 0 && t == solutions[s - 1]))
        {
            // make a move
            solutions[s] = t;
            move(t);

            if (won(offset))
            {
                return s + 1;
            }

            // if won after this move
            if ((total_s = brute(offset, s + 1, solutions)))
            {
                return total_s;
            }

            // if neither, then moving right is bad. we need to undo this
            move(t);
        }
    }

    // try left
    if (e_j > offset)
    {
        t = board[e_i][e_j - 1];

        if (!(s > 0 && t == solutions[s - 1]))
        {
            // make a move
            solutions[s] = t;
            move(t);

            if (won(offset))
            {
                return s + 1;
            }

            // if won after this move
            if ((total_s = brute(offset, s + 1, solutions)))
            {
                return total_s;
            }

            // if neither, then moving left is bad. we need to undo this
            move(t);
        }
    }

    // by this point we have tried all to no avail
    return 0;
}

void get_position(int tile, int *reti, int* retj)
{
    int i, j;

    for (i = 0; i < d; i++)
    {
        for (j = 0; j < d; j++)
        {
            if (board[i][j] == tile)
            {
                *reti = i;
                *retj = j;
                return;
            }
        }
    }
}

// DANGER: god moves don't check...
void god_move_up()
{
    board[e_i][e_j] = board[e_i - 1][e_j];
    board[e_i - 1][e_j] = 0;
    e_i--;
    usleep(250000);
    clear();
    draw();
}

void god_move_down()
{
    board[e_i][e_j] = board[e_i + 1][e_j];
    board[e_i + 1][e_j] = 0;
    e_i++;
    usleep(250000);
    clear();
    draw();
}

void god_move_left()
{
    board[e_i][e_j] = board[e_i][e_j - 1];
    board[e_i][e_j - 1] = 0;
    e_j--;
    usleep(250000);
    clear();
    draw();
}

void god_move_right()
{
    board[e_i][e_j] = board[e_i][e_j + 1];
    board[e_i][e_j + 1] = 0;
    e_j++;
    usleep(250000);
    clear();
    draw();
}

void god_move_to_i(int desti)
{
    while(e_i < desti)
    {
        god_move_down();
    }

    while(e_i > desti)
    {
        god_move_up();
    }
}

void god_move_to_j(int destj)
{
    while(e_j < destj)
    {
        god_move_right();
    }

    while(e_j > destj)
    {
        god_move_left();
    }
}

void god_move(int tile)
{
    move(tile);
    usleep(250000);
    clear();
    draw();
}

void god_solve(int cur_d)
{
    int init = d - cur_d;
    int i, j;

    if (cur_d == 3)
    {
        // let's brute force it
        int temp_board[3][3];
        int solutions[32];
        int oldei, oldej;
        // save current config
        for (i = 0; i < 3; i++)
        {
            for (j = 0; j < 3; j++)
            {
                temp_board[i][j] = board[i + init][j + init];
            }
        }
        oldei = e_i;
        oldej = e_j;

        int solved = brute(init, 0, solutions);

        // restore the board
        for(i = 0; i < 3; i++)
        {
            for(j = 0; j < 3; j++)
            {
                board[i + init][j + init] = temp_board[i][j];
            }
        }

        e_i = oldei;
        e_j = oldej;

        for(i = 0; i < solved; i++)
        {
            god_move(solutions[i]);
        }

        printf("Solved! Thank you for using God mode.\n");
    }
    else
    {
        // set first row
        // with init, this means our first row is init_th row
        // with (init, init), (init, init+1), ...
        // so it's init * d + init, init * d + init  + 1...
        int curi, curj;
        int targetii = init;
        int targetjj;
        int targeti;
        int targetj;
        int tile;

        for (targetjj = init; targetjj < d; targetjj++)
        {
            tile = targetii * d + targetjj + 1;
            get_position(tile, &curi, &curj);

            if (targetjj == d - 2)
            {
                targetj = targetjj + 1;
            }
            else
            {
                targetj = targetjj;
            }

            if (targetjj == d - 1)
            {
                targeti = targetii + 1;
            }
            else
            {
                targeti = targetii;
            }

            // hack
            if (targetjj == d - 1 && curi == init + 1 && curj == d - 2)
            {
                god_move_right();
                god_move_down();
                god_move_down();
                god_move_left();
                god_move_up();
                god_move_right();
                god_move_up();
                god_move_left();
                god_move_down();
                god_move_down();
                god_move_right();
                god_move_up();
                god_move_left();
                continue;
            }

            // move space to below the tile
            if (curi == d - 1)
            {
                god_move_to_i(curi - 1);
                god_move_to_j(curj);
                god_move_down();
                curi--;
            }
            else if (curj == e_j)
            {
                if (curi > e_i)
                {
                    god_move_to_i(curi);
                    curi--;
                }
                else
                {
                    god_move_to_i(curi + 1);
                }
            }
            else
            {
                god_move_to_i(curi + 1);
                god_move_to_j(curj);
            }

            // move tile horizontally
            while (curj > targetj)
            {
                god_move_left();
                god_move_up();
                god_move_right();
                god_move_down();
                god_move_left();
                curj--;
            }

            while (curj < targetj)
            {
                god_move_right();
                god_move_up();
                god_move_left();
                god_move_down();
                god_move_right();
                curj++;
            }

            // move tile upward
            if (targetj != d - 1)
            {
                god_move_right();
                god_move_up();
                while (curi > targeti)
                {
                    god_move_up();
                    god_move_left();
                    god_move_down();
                    god_move_right();
                    god_move_up();
                    curi--;
                }
            }
            else
            {
                god_move_left();
                god_move_up();

                while (curi > targeti)
                {
                    god_move_up();
                    god_move_right();
                    god_move_down();
                    god_move_left();
                    god_move_up();
                    curi--;
                }
            }

        }

        // fix the end
        god_move_up();
        god_move_right();
        god_move_down();

        // set first column
        targetjj = init;

        for (targetii = init + 1; targetii < d; targetii++)
        {
            tile = targetii * d + targetjj + 1;

            get_position(tile, &curi, &curj);

            if (targetii == d - 2)
            {
                targeti = targetii + 1;
            }
            else
            {
                targeti = targetii;
            }

            if (targetii == d - 1)
            {
                targetj = targetjj + 1;
            }
            else
            {
                targetj = targetjj;
            }

            // hack
            if (targetii == d - 1 && curj == init + 1 && curi == d - 2)
            {
                god_move_down();
                god_move_right();
                god_move_right();
                god_move_up();
                god_move_left();
                god_move_down();
                god_move_left();
                god_move_up();
                god_move_right();
                god_move_right();
                god_move_down();
                god_move_left();
                god_move_up();
                continue;
            }

            // move space to right of the tile
            if (curj == d - 1)
            {
                god_move_to_j(curj - 1);
                god_move_to_i(curi);
                god_move_right();
                curj--;
            }
            else if (curi == e_i)
            {
                if (curj > e_j)
                {
                    god_move_to_j(curj);
                    curj--;
                }
                else
                {
                    god_move_to_j(curj + 1);
                }
            }
            else
            {
                god_move_to_j(curj + 1);
                god_move_to_i(curi);
            }

            // move tile vertically
            while (curi > targeti)
            {
                god_move_up();
                god_move_left();
                god_move_down();
                god_move_right();
                god_move_up();
                curi--;
            }

            while (curi < targeti)
            {
                god_move_down();
                god_move_left();
                god_move_up();
                god_move_right();
                god_move_down();
                curi++;
            }

            // move tile leftward
            if (targeti != d - 1)
            {
                god_move_down();
                god_move_left();

                while (curj > targetj)
                {
                    god_move_left();
                    god_move_up();
                    god_move_right();
                    god_move_down();
                    god_move_left();
                    curj--;
                }
            }
            else
            {
                god_move_up();
                god_move_left();

                while (curj > targetj)
                {
                    god_move_left();
                    god_move_down();
                    god_move_right();
                    god_move_up();
                    god_move_left();
                    curj--;
                }
            }

        }

        // fix the end
        god_move_left();
        god_move_down();
        god_move_right();
        god_solve(cur_d - 1);
    }
}
