#include "wish.h"

/**
 * 	Displays shell prompt
 **/
void prompt()
{
	printf("wish> ");
}

/**
 * 	Gets a command from the user
 * 
 * 	Params:
 * 		char* tokens[] - the array of tokens to be filled
 *
 * 	Returns:
 * 		The number of tokens
 **/
int getCommand(char* tokens[])
{
	// Display shell prompt if interactive mode
	if (INPUT_FILE == stdin)
		prompt();

	// Get line of input
	size_t size = 0;
	char* line = NULL;
	getline(&line, &size, INPUT_FILE);

	// Tokenize the command
	char* token = NULL;
	// char* to_process = strdup(line);
	int i = 0;
	while ( (token = strsep(&line, " ")) != NULL)
	{
		// Check if we need to split a pipe
		char* ptr_cmp = strchr(token, '>');
		if (ptr_cmp != NULL)
		{
			// Grab the left-hand token (if it exists!)
			tokens[i] = strsep(&token, ">");
			if (strlen(tokens[i]) > 0)
				i++;
			// Add the pipe as its own token
			tokens[i] = ">";
			i++;
			// Add the right-hand token (if it exists!)
			if (strlen(token) > 0)
			{
				tokens[i] = token;
				i++;
			}
		}
		// No pipe to split, this is a clean token
		else
		{
			tokens[i] = token;
			i++;
		}
	}
	// Remove newline character on last token
	tokens[i-1] = strsep(&tokens[i-1], "\n");
	return i;
}




int main(int argc, char *argv[])
{
	// Disable the stdout buffer for debugging
	if (DEBUG)
		setvbuf(stdout, NULL, _IONBF, 0);
	
	// Determine whether we are in interactive mode
	if (argc <= 1)
		INPUT_FILE = stdin;
	else
		INPUT_FILE = fopen(argv[1], "r");
		

	// Main shell loop
	// while (1)
	int loop;
	for (loop = 0; loop < 4; loop++)
	{
		// Get next command
		char *tokens[MAX_NUM_TOKENS];
		int num_tokens = getCommand(tokens);

		// Display parsed tokens
		for (int i = 0; i < num_tokens; i++)
		{
			if(tokens[i] == NULL)
				break;
			printf("%s ", tokens[i]);
		}
		printf("\n");
	}
	return 0;
}

