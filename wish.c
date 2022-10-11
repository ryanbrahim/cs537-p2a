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
	int num_args = num_tokens - 1;
	char** argv = malloc( sizeof(char*) * num_tokens);
	for (int i = 1; i < num_tokens; i++)
		argv[i] = strdup(tokens[i+1]);
	argv[num_tokens] = NULL;

	// Exit condition
	if(strcmp(command, "exit") == 0)
		exit(0);

	// Check if built-in command
	// if(strcmp(command, "cd") == 0)
	// 	cd(tokens[1]);
	// if(strcmp(command, "path") == 0)
	// 	path(tokens);

	// Check if program in path
	int num_paths = 1;	// TODO: Make dynamic
	for( int i=0; i < num_paths; i++)
	{
		// Build possible program path
		strcpy(prog_path, BIN_PATHS[i]);
		strcat(prog_path, "/");
		strcat(prog_path, command);
		printf("Trying to access %s\n", prog_path);

		// Check if that program path exists
		if( access(prog_path, X_OK) == 0 )
		{
			// Execute that program!
			printf("Executing %s\n", prog_path);
			pid_t pid = fork();
			if( pid == 0 )
			{
				argv[0] = prog_path;
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
	
	char* command = malloc( sizeof(char*) * (MAX_LINE_LENGTH) );
	char** command_args = malloc( sizeof(char*) * (MAX_NUM_TOKENS) );

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

