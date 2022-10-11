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
 * 	exit built-in
*/
builtin_exit(char* tokens[], int num_tokens)
{
	if(num_tokens > 1)
		error();
	else	
		exit(0);
}


/**
 * cd build-in function.  Changes directory
*/
void builtin_cd(char* tokens[], int num_tokens)
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
void builtin_path(char* tokens[], int num_tokens)
{
	int new_paths = num_tokens - 1;
	for (int i = 0; i < new_paths; i++)
		BIN_PATHS[i] = tokens[1+i]; 
	NUM_PATHS = new_paths;
}


/**
 *  Handles redirection by updating the stdout file
 *  	to a provided file.  The provided file is the token
 * 		immediately after the redirect index.
 * 
 * 	Returns true if handled, false if not
*/
bool handleRedirect(char* tokens[], int num_tokens, int redirect_index)
{
	// ERROR: Trying to redirect, but no (or too many) output file given
	if ( redirect_index + 1 != num_tokens - 1 )
	{
		error();
		return false;
	}

	char* output_file = tokens[redirect_index+1];
	int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, fileno(stdout));
    close(fd);  
	return true;
}


COMMAND_T determineCommand(char* tokens[], int num_tokens)
{
	char* command = tokens[0];
	if ( strcmp(command,strdup("exit")) == 0 
			|| strcmp(command,strdup("cd")) == 0 
			|| strcmp(command,strdup("path")) == 0 )
	{
		return BUILTIN;
	}

	
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
	// char prog_path[MAX_PATH_LENGTH] = "";
	char* prog_path = malloc( sizeof(char) * MAX_PATH_LENGTH );

	// Get the command and it's args
	char* command = strdup(tokens[0]);
	char** argv = malloc( sizeof(char*) * num_tokens + 1);
	argv[0] = command;
	int argc = 1;
	bool is_redirect = false;
	for (int i = 1; i < num_tokens; i++)
	{
		char* token = strdup(tokens[i]);
		// If we hit a redirect, handle redirect, and stop reading args
		if ( strcmp(token, strdup(">")) == 0 )
		{
			is_redirect = true;
			if (!handleRedirect(tokens, num_tokens, i))
				return;
			break;
		}
		// Add this token to the command args
		argv[i] = token;
		argc++;
	}
	argv[argc] = NULL;


	// Check if built-in command
	if(strcmp(command, "exit") == 0)
	{
		builtin_exit(tokens, num_tokens);
		return;
	}
	if(strcmp(command, "cd") == 0)
	{
		builtin_cd(tokens, num_tokens);
		return;
	}
	if(strcmp(command, "path") == 0)
	{
		builtin_path(tokens, num_tokens);
		return;
	}


	// Check if program command in path
	for( int i=0; i < NUM_PATHS; i++)
	{
		// Build possible program path
		strcpy(prog_path, BIN_PATHS[i]);
		strcat(prog_path, "/");
		strcat(prog_path, command);
		// argv[0] = prog_path;
		// printf("Trying to access %s\n", prog_path);

		// Check if that program path exists
		if( access(prog_path, X_OK) == 0 )
		{
			pid_t pid = fork();
			if( pid == 0 )
			{
				if(is_redirect)
					handleRedirect;
				execv(prog_path, argv);
			}
			int status;
			waitpid(pid, &status, 0);
			// Successfully ran program!  We can return now
			return;
		}
		else
			continue;
	}
	// Could not find program, error
	error();
}


int main(int argc, char *argv[])
{
	// ERROR: Passed more than one file
	if ( argc >= 3 )
		exit(1);
	// ERROR: Bad batch file
	if ( argc == 2 &&  access(argv[1], R_OK) != 0)
		exit(1);

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

