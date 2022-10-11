#include "wish.h"

/**
 * 	Displays shell prompt
 **/
void prompt()
{
	printf("wish> ");
}

/**
 * Displays error message
*/
void error()
{
	char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message)); 
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
	// Make null-terminated
	if ( strlen(tokens[i-1]) == 0 )
	{
		tokens[i-1] = NULL;
		i--;
	}
	else
		tokens[i] = NULL;
	return i;
}

/**
 * cd build-in function.  Changes directory
*/
void cd(char* tokens[], int num_tokens)
{
	// Error checking
	if (num_tokens != 2)
		error();
	// Valid command, change directory
	else
		chdir(strdup(tokens[1]));
}


/**
 * Path built-in function.  Adds a new path directory
 * 	to our list of paths.
 * 
 * 	Params:
 * 		tokens[] - the command tokens
 * 		int num_tokens - the number of tokens
*/
void path(char* tokens[], int num_tokens)
{
	int new_paths = num_tokens - 1;
	for (int i = 0; i < new_paths; i++)
		BIN_PATHS[i] = tokens[1+i]; 
	NUM_PATHS = new_paths;
}


/**
 * 	Execute a command.
 * 
 * 	Parameters:
 * 		char* tokens[] - array of tokens
 */
void executeCommand(char* tokens[], int num_tokens)
{
	// Local string vars
	char prog_path[MAX_PATH_LENGTH] = "";

	// Get the command and it's args
	char* command = strdup(tokens[0]);
	char** argv = malloc( sizeof(char*) * num_tokens + 1);
	for (int i = 1; i < num_tokens; i++)
		argv[i] = strdup(tokens[i]);
	argv[num_tokens] = NULL;


	// Check if built-in command
	if(strcmp(command, "exit") == 0)
		exit(0);
	if(strcmp(command, "cd") == 0)
	{
		cd(tokens, num_tokens);
		return;
	}
	if(strcmp(command, "path") == 0)
	{
		path(tokens, num_tokens);
		return;
	}


	// Check if program command in path
	for( int i=0; i < NUM_PATHS; i++)
	{
		// Build possible program path
		strcpy(prog_path, BIN_PATHS[i]);
		strcat(prog_path, "/");
		strcat(prog_path, command);
		argv[0] = prog_path;
		// printf("Trying to access %s\n", prog_path);

		// Check if that program path exists
		if( access(prog_path, X_OK) == 0 )
		{
			// printf("Pre-executing %s\n", prog_path);
			// printf("\targv =");
			// for (int i = 0; i <= num_tokens; i++)
					// printf(" %s", argv[i]);
			// printf("\n");

			// Execute that program!
			// printf("Executing %s\n", prog_path);

			pid_t pid = fork();
			if( pid == 0 )
			{
				// printf("\tpid: %d\n", (int)pid);
				// printf("\tprog_path: %s\n", prog_path);
				// printf("\targv =");
				// for (int i = 0; i <= num_tokens; i++)
					// printf(" %s", argv[i]);
				// printf("\n");
				execv(prog_path, argv);
			}
			int status;
			waitpid(pid, &status, 0);
		}
		else
			continue;
	}
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

	// Intialize global variable for BIN_PATHS 
	BIN_PATHS = malloc( sizeof(char*) * MAX_PATHS );
	BIN_PATHS[0] = "/bin";
	NUM_PATHS = 1;
	
	// Main shell loop
	while (1)
	{
		// Get tokens from next line
		char *tokens[MAX_NUM_TOKENS];
		int num_tokens = getTokens(tokens);
		// Execute that tokenized command
		executeCommand(tokens, num_tokens);
	}
	return 0;
}

