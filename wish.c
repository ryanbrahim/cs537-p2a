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
int getTokens(char* tokens[])
{
	// Display shell prompt if interactive mode
	if (INPUT_FILE == stdin)
		prompt();

	// Get line of input
	size_t size = 0;
	char* line = NULL;
	int chars_read = getline(&line, &size, INPUT_FILE);

	// Hit end of file?
	if(chars_read == -1)
	{
		tokens[0] = "exit";
		return 1;
	}

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

/**
 * 	Parse tokens into a valid command (if possible)
 * 
 * 	Parameters:
 * 		char* tokens[] - an array of command tokens
 * 		int num_tokens - the number of tokens
 * 
 * 
 * 	Returns:
 * 		true if valid command, false otherwise
 */
bool parseCommand(char* tokens[], int num_tokens, char command[], char* args[])
{
	command = tokens[0];
	args = malloc( sizeof(char*) * (num_tokens-1) );
	int i;
	for(i = 1; i < num_tokens; i++)
		args[i-1] = tokens[i];
	return true;
}

/**
 * 	Execute a command.
 * 
 * 	Parameters:
 * 		char command[] - the command
 * 		char* args[] - the command's arguments
 */
void executeCommand(char command[], char* args[])
{

	// Exit condition
	if(strcmp(command, "exit") == 0)
		exit(0);

	// Different command?
	printf("The command is : %s\n", command);

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
	while (1)
	{
		// Get tokens from next line
		char *tokens[MAX_NUM_TOKENS];
		int num_tokens = getTokens(tokens);
		// Parse tokens into valid command
		char* command;
		char* command_args[];


		if(parseCommand(tokens, num_tokens, command, command_args))
		{
			// Execute that command
			executeCommand(command, command_args);
		}

		

		// // Display parsed tokens
		// for (int i = 0; i < num_tokens; i++)
		// {
		// 	printf("%s ", tokens[i]);
		// }
		// printf("\n");
	}
	return 0;
}

