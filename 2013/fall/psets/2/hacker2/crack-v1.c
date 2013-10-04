#define _POSIX_C_SOURCE 200809L
#include <crypt.h>
#include <cs50.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SALT_LEN 2
#define HASH_LEN 11

/* Crypt only uses the first 8 characters. */
#define MAX_GUESS_LEN 8

/* Min and max of displayable characters. */
#define ASCII_CHAR_MIN ' '
#define ASCII_CHAR_MAX '~'
#define ASCII_TOTAL (ASCII_CHAR_MAX - ASCII_CHAR_MIN)

/* The default dictionary file. It's not particularily good. */
#define DICTIONARY_FILE "/usr/share/dict/american-english"

/* Status of the guess. */
#define SUCCESS 0x00000000
#define FAILURE 0x00000001


/**************************************************************************
 * FUNCTIONS DECLARATIONS
 **************************************************************************/

/* Does a dictionary attack based on DICTIONARY_FILE. */
int dictionary_search(char *hash, char *salt);

bool DoSearch(char* CurrentResult, char salt[], int CurrentIndex, char hash[], int numchars);

bool DoAlphaNumericSearch(char* CurrentGuess, char* salt, int CurrentIndex, char* hash, int numchars);

bool IsAlphaNumeric(char* CurrentGuess, int numchars);

/**************************************************************************
 * MAIN
 **************************************************************************/

/* Main function. */
int main(int argc, char *argv[])
{
  char salt[SALT_LEN + 1];
  char CurrentGuess[MAX_GUESS_LEN + 1];

  int i, j;

  /* Check input parameters. */
  if(argc != 2 || strlen(argv[1]) != SALT_LEN + HASH_LEN) {
    printf("Usage: %s DES-HASH\n", argv[0]);
    return -1;
  }

  /* Copy over the salt and null-terminate the seed string. */
  strncpy(salt, argv[1], SALT_LEN);
  salt[SALT_LEN] = '\0';

  /* Guesses the dictionary. */
  if(dictionary_search(argv[1], salt) == SUCCESS) {
    return 0;
  }

  for(i=0; i< MAX_GUESS_LEN; i++)
    {
	CurrentGuess[i] = ASCII_CHAR_MIN;
    }
  CurrentGuess[MAX_GUESS_LEN] = '\0';

  /* Check alphanumeric */

  for (j=1; j<= MAX_GUESS_LEN; j++)
  {
      if(DoAlphaNumericSearch(CurrentGuess, salt, j-1, argv[1], j) == true)
      {
	  return 0;
      }
      for(i=0; i<MAX_GUESS_LEN; i++)
      {
	  CurrentGuess[i] = ASCII_CHAR_MIN;
      }
      CurrentGuess[MAX_GUESS_LEN] = '\0';
  }

/* Check all passwords with j characters */
  for(j=1; j<= MAX_GUESS_LEN; j++)
    {
	if(DoSearch(CurrentGuess, salt, j - 1, argv[1], j) == true)
	{
	    return 0;
	}
	for(i=0; i< MAX_GUESS_LEN; i++)
	{
	    CurrentGuess[i] = ASCII_CHAR_MIN;
	}
	CurrentGuess[MAX_GUESS_LEN] = '\0';
	
    }
    
    printf("Could not find password.\n");
    return 1;

}

/* Does a dictionary attack based on DICTIONARY_FILE.*/
int dictionary_search(char *hash, char *salt)
{
  FILE *dict_file;
  char *guess = NULL, *guess_hash;
  size_t mem_len, read;

  /* Attempt to open the file.*/
  dict_file = fopen(DICTIONARY_FILE, "r");
  if (dict_file == NULL) {
    return FAILURE;
  }

  /* Getline automatically allocates or reallocates memory. */
  while ((read = getline(&guess, &mem_len, dict_file)) > 0) {
    /* Only read the newline, so continue. */
    if(read == 1) {
      continue;
    }

    /* Remove the newline at the end and generate a hash. */
    guess[read - 1] = '\0';
    guess_hash = crypt(guess, salt);

    if(!strcmp(guess_hash, hash)) {
      printf("Password: %s\n", guess);
      free(guess);

      return SUCCESS;
    }
  }

  /* Free the array if it's still around. */
  free(guess);
  return FAILURE;
}

/* Search for alphanumeric passwords */
bool DoAlphaNumericSearch(char* CurrentGuess, char* salt, int CurrentIndex, char* hash, int numchars)
{
    char CurrentChar;
    char* CurrentResult;
    char CurrentSubString[MAX_GUESS_LEN+1];

    if(CurrentIndex == 0)
    {
	CurrentChar = '0';
	while(CurrentChar <= 'z')
	{
	    CurrentGuess[CurrentIndex] = CurrentChar;
	    strncpy(CurrentSubString, CurrentGuess, numchars);
	    CurrentSubString[numchars] = '\0';
	    CurrentResult = crypt(CurrentSubString, salt);
	    if (strcmp(CurrentResult, hash) == 0)
	    {
		printf("Password: %s\n", CurrentSubString);
		return true;
	    }
	    switch(CurrentChar)
	    {
	      case '9':
		CurrentChar = 'A';
		break;
	      case 'Z':
		CurrentChar = 'a';
		break;
	      default:
		CurrentChar++;
	    }
	}
	    
    }
    else {
	CurrentChar = '0';
	while(CurrentChar <= 'z')
	{
	    CurrentGuess[CurrentIndex]=CurrentChar;
	    if(DoAlphaNumericSearch(CurrentGuess, salt, CurrentIndex - 1, hash, numchars) == true)
	    {
		return true;
	    }
	    switch(CurrentChar)
	    {
	      case '9':
		CurrentChar = 'A';
		break;
	      case 'Z':
		CurrentChar = 'a';
		break;
	      default:
		CurrentChar++;
	    }
	}
    }

    return false;
}

/* Since we've already checked all alphanumeric passwords, this only checks passwords that have at least one special character in them. */
bool DoSearch(char* CurrentGuess, char* salt, int CurrentIndex, char* hash, int numchars)
{
    char CurrentChar;
    char* CurrentResult;
    char CurrentSubString[MAX_GUESS_LEN+1];

    /* for the zeroth index, we just cycle through the possiblities and check.  If the password is alphanumeric up to this point, then we only cycle through the non-alphanumeric possibilities for this last character. */
    if(CurrentIndex == 0)
    {
	if(IsAlphaNumeric(CurrentGuess, numchars))
	{
	    CurrentChar = ASCII_CHAR_MIN;
	    while(CurrentChar <= ASCII_CHAR_MAX)
	    {
		CurrentGuess[CurrentIndex]=CurrentChar;
		strncpy(CurrentSubString, CurrentGuess, numchars);
		CurrentSubString[numchars] = '\0';
		CurrentResult = crypt(CurrentSubString, salt);
		if (strcmp(CurrentResult, hash) == 0)
		{
		    printf("Password: %s\n", CurrentSubString);
		    return true;
		}
                switch(CurrentChar)
                {
                  case '/':
                    CurrentChar = ':';
                    break;
                  case '@':
                    CurrentChar = '[';
                    break;
                  case '`':
                    CurrentChar = '{';
                    break;
                  default:
                    CurrentChar++;
                }
	    }
	    return false;
	}
	else {

	    for (CurrentChar = ASCII_CHAR_MIN; CurrentChar <= ASCII_CHAR_MAX; CurrentChar++)
	    {
		CurrentGuess[CurrentIndex]=CurrentChar;
		strncpy(CurrentSubString, CurrentGuess, numchars);
		CurrentSubString[numchars] = '\0';
		CurrentResult = crypt(CurrentSubString, salt);
		if (strcmp(CurrentResult, hash) == 0)
		{
		    printf("Password: %s\n", CurrentSubString);
		    return true;
		}
	    }
	    
	    return false;
	}
    }
    /* for the other indices, we cycle through the possibilities and make a recursive call */
    else
    {
	for(CurrentChar = ASCII_CHAR_MIN; CurrentChar <= ASCII_CHAR_MAX; CurrentChar++)
	{
	    CurrentGuess[CurrentIndex]=CurrentChar;
	    if(DoSearch(CurrentGuess, salt, CurrentIndex - 1, hash, numchars) == true)
	    {
		return true;
	    }
	}

    }

    return false;
}

/* Check whether the string is alphanumeric, except the zeroth char */
bool
IsAlphaNumeric(char *CurrentGuess, int numchars)
{
    for (int i = 1; i < numchars; i++)
	if (!isalnum(CurrentGuess[i]))
	    return false;

    return true;
}
