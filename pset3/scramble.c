/***************************************************************************
 * scramble.c
 *
 * Problem Set 3
 *
 * Implements Scramble with CS50.
 *
 * Usage: scramble [#]
 *
 * where # is an optional board number.
 ***************************************************************************/
 
#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// duration of a game in seconds
#define DURATION 30

// board's dimensions
#define DIMENSION 4

// board
char board[DIMENSION][DIMENSION];

// flags with which we can mark board's letters while searching for words
bool marks[DIMENSION][DIMENSION];

// maximum number of words in any dictionary
#define WORDS 172806

// maximum number of letters in any word
// (e.g., pneumonoultramicroscopicsilicovolcanoconiosis)
#define LETTERS 45

// defines a word as having an array of letters plus a flag
// indicating whether word has been found on board
typedef struct
{
    bool found;
    char letters[LETTERS];
}
word;

// defines a dictionary as having a size and an array of words
struct
{
    int size;
    word words[WORDS];
}
dictionary;

// prototypes
void clear(void);
void draw(void);
bool find(string word);
void initialize(void);
bool load(string filename);
bool lookup(string word);
bool crawl(string word, int x, int y);

// This is Scramble.
int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc > 2)
    {
        printf("Usage: scramble [#]\n");
        return 1;
    }

    // seed pseudorandom number generator
    if (argc == 2)
    {
        int seed = atoi(argv[1]);
        if (seed < 0)
        {
            printf("Illegal seed.\n");
            return 1;
        }
        srand(seed);
    }
    else
    {
        srand(time(NULL));
    }

    // load dictionary
    if (!load("./dictionary.csv"))
    {
        printf("Could not open dictionary.\n");
        return 1;
    }

    // initialize user's score
    int score = 0;

    // initialize the board
    initialize();

    // calculate time of game's end
    int end = time(NULL) + DURATION;

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // get current time
        int now = time(NULL);

        // report score
        printf("Score: %d\n", score);

        // check for game's end
        if (now >= end)
        {
            printf("\033[31m"); // red
            printf("Time:  %d\n\n", 0);
            printf("\033[39m"); // default
            return 0;
        }

        // report time remaining
        printf("Time:  %d\n\n", end - now);
        
        // prompt for word
        printf("> ");
        string word = GetString();
        if (word != NULL)
        {
            // capitalize word
            for (int i = 0, n = strlen(word); i < n; i++) 
                word[i] = toupper(word[i]);

            // look for word on board and in dictionary
            if (find(word) == true && lookup(word) == true)
                score += strlen(word);
        }
    }

    // the end
    return 0;
}

/**
 * Clears screen.
 */
void clear()
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Crawls board recursively for letters starting at board[x][y].
 * Returns true iff all letters are found.
 */
bool crawl(string letters, int x, int y)
{
    // if out of letters, then we must've found them all!
    if (strlen(letters) == 0)
        return true;

    // don't fall off the board!
    if (x < 0 || x >= DIMENSION)
        return false;
    if (y < 0 || y >= DIMENSION)
        return false;

    // been here before!
    if (marks[x][y] == true)
        return false;

    // mark location as seen
    marks[x][y] = true;

    // check board[x][y] for current letter
    if (board[x][y] != letters[0])
        return false;

    // look left and right for next letter
    for (int i = -1; i <= 1; i++)
    {
        // look down and up for next letter
        for (int j = -1; j <= 1; j++)
        {
            // don't check board[x+0][y+0]
            if (i == 0 && j == 0)
                continue;

            // check board[x+i][y+j] for next letter
            if (crawl(&letters[1], x+i, y+j) == true)
                return true;
        }
    }
    return false;
}

/** 
 * Prints the board in its current state.
 */
void draw(void)
{
    printf("\n");
	for (int i = 0; i < DIMENSION; i++)
    {
        printf(" ");
	    for (int j = 0; j < DIMENSION; j++)
        {
            printf("%2c", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

/**
 * Returns true iff word is found in board.
 */
bool find(string word)
{
    // word must be at least 2 characters in length
    if (strlen(word) < 2)
        return false;

    // search board for word
    for (int i = 0; i < DIMENSION; i++)
    {
        for (int j = 0; j < DIMENSION; j++)
        {
            // reset marks
            for (int i = 0; i < DIMENSION; i++)
                for (int j = 0; j < DIMENSION; j++)
                    marks[i][j] = false;

            // search for word starting at board[i][j]
            if (crawl(word, i, j) == true)
                return true;
        }
    }
    return false;
}

/**
 * Initializes board with letters.
 */
void initialize(void)
{
    // http://en.wikipedia.org/wiki/Letter_frequency#Relative_frequencies_of_letters_in_the_English_language
    float frequencies[26] = {
     8.167,  // a
     1.492,  // b
     2.782,  // c
     4.253,  // d
     12.702, // e
     2.228,  // f
     2.015,  // g
     6.094,  // h
     6.966,  // i
     0.153,  // j
     0.747,  // k
     4.025,  // l
     2.406,  // m
     6.749,  // n
     7.507,  // o
     1.929,  // p
     0.095,  // q
     5.987,  // r
     6.327,  // s
     9.056,  // t
     2.758,  // u
     1.037,  // v
     2.365,  // w
     0.150,  // x
     1.974,  // y
     0.074   // z
    };

    // iterate over board
	for (int i = 0; i < DIMENSION; i++)
    {
	    for (int j = 0; j < DIMENSION; j++)
        {   
            // generate pseudorandom double in [0, 1)
            double d = rand() / (double) RAND_MAX;

            // map d onto range of frequencies
            for (int k = 0; k < 26; k++)
            {
                d -= frequencies[k] / 100;
                if (d < 0.0 || k == 25)
                {
                    board[i][j] = 'A' + k;
                    break;
                }
            }
        }
    }
}

/**
 * Loads words from dictionary with given filename into a global array.
 */
bool load(string filename)
{
    // open dictionary
    FILE* file = fopen(filename, "r");
    if (file == NULL)
        return false;

    // initialize dictionary's size
    dictionary.size = 0;

    // load words from dictionary
    char buffer[LETTERS+1];
    while (fgets(buffer, LETTERS+1, file))
    {
        // overwrite \n with \0
        buffer[strlen(buffer) - 1] = '\0';

        // capitalize word
        for (int i = 0, n = strlen(buffer); i < n; i++)
            buffer[i] = toupper(buffer[i]);

        // copy word into dictionary
        dictionary.words[dictionary.size].found = false;
        strncpy(dictionary.words[dictionary.size].letters, buffer, LETTERS+1);
        dictionary.size++;
    }

    // success!
    return true;
}

/**
 * Looks up word in dictionary.  Iff found, flags word
 * and returns true.
 */
bool lookup(string word)
{
    // iterate over words in dictionary
    for (int i = 0; i < dictionary.size; i++)
    {
        // check if found
        if (strcmp(dictionary.words[i].letters, word) == 0)
        {
            // check if already found previously
            if (dictionary.words[i].found == true)
                break;

            // flag as found
            dictionary.words[i].found = true;

            // found it!
            return true;
        }
    }

    // fail
    return false;
}
