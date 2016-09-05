#include <stdio.h>
#include <stdlib.h>
#include <cs50.h>
#include <ctype.h>

char decrypt(char message_char, string keyword, int key_pos);

int main(int argc, char* argv[]) 
{
    int i = 0;    // standard counter var
    char temp_c;  // temporary char var

    string keyword;    // cs50 string for storing keyword
    int    key_length; // length of keyword
    string message;    // user message to encrypt

    int keyword_counter = 0;  // tracks position in keyword

    // check for proper cmd line args
    if(argc != 2 )
    {
	printf("Usage: devigenere <keyword>\n");
	return -1;
    }
    
    // check that keyword is only letters
    while((temp_c = argv[1][i]) != '\0')
    {
	if(!isalpha(temp_c))
	{
	    printf("Keyword must be only A-Z and a-z\n");
	    return -1;
	}

	i++;
    }

    // if everything is okay, assign keyword to nicer variable, all lowercase
    keyword = argv[1];

    // Save length of keyword
    key_length = i;

    // Convert keyword to values 0 - 25
    for(i=0; keyword[i] != '\0'; i++)
    {
	keyword[i] = tolower(keyword[i])-97;
    }

    // Get message from user
    printf("Ciphertext: ");
    message = GetString();

    // Decrypt message by overwriting non-encrypted message
    for(i=0; message[i] != '\0'; i++)
    {
	// Only decrypt if character is in alphabet
	if(isalpha(message[i]))
	{
	    message[i] = decrypt(message[i], keyword, keyword_counter);
	    keyword_counter = (keyword_counter+1)%key_length;
	}
    }

    printf("Plaintext:  %s\n", message);

}

// Encrypts a single character
char decrypt(char message_char, string keyword, int key_pos)
{
    bool lowercase = islower(message_char);  // is char upper or lower case

    // Represent character as 0-25
    int rep26 = (lowercase ? (message_char-97) : (message_char-65));

    // Return decrypted character (add 26 to account for negative #'s)
    return ((lowercase) ? (rep26 - keyword[key_pos]+26)%26 + 97 :
	                  (rep26 - keyword[key_pos]+26)%26 + 65  );
}
