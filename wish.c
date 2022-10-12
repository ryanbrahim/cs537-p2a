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
	while ( (token = strsep(&line, " \t\n")) != NULL)
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
		// Empty line, skip it
		else if ( strcmp(token, strdup("")) == 0 ) {}
		// No pipe to split, this is a clean token
		else
		{
			tokens[i] = token;
			i++;
		}
	}
	// Remove newline character on last token
	// tokens[i-1] = strsep(&tokens[i-1], "\n");
	// Make null-terminated
	if ( i > 0 && strlen(tokens[i-1]) == 0 )
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
int builtinExit(char* tokens[], int num_tokens)
{
	if(num_tokens > 1)
		return false;
	else	
		exit(0);
	return 0;
}


/**
 * cd build-in function.  Changes directory
*/
int builtinCd(char* tokens[], int num_tokens)
{
	// Error checking
	if (num_tokens != 2)
		return false;
	// Valid command, change directory
	else
		chdir(strdup(tokens[1]));
	return 0;
}


/**
 * Path built-in function.  Adds a new path directory
 * 	to our list of paths.
 * 
 * 	Params:
 * 		tokens[] - the command tokens
 * 		int num_tokens - the number of tokens
*/
int builtinPath(char* tokens[], int num_tokens)
{
	int new_paths = num_tokens - 1;
	for (int i = 0; i < new_paths; i++)
		BIN_PATHS[i] = tokens[1+i]; 
	NUM_PATHS = new_paths;
	return 0;
}


/**
 *  Handles redirection by updating the stdout file
 *  	to a provided file.  The provided file is the token
 * 		immediately after the redirect index.
 * 
 * 	Returns true if handled, false if not
*/
int handleRedirect(char* tokens[], int num_tokens, int redirect_index)
{
	// ERROR: Trying to redirect, but no (or too many) output file given
	if ( redirect_index + 1 != num_tokens - 1 )
	{
		error();
		return 1;
	}

	char* output_file = tokens[redirect_index+1];
	int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, fileno(stdout));
    close(fd);  
	return 0;
}


COMMAND_T determineCommand(char* tokens[], int num_tokens)
{
	char* command = tokens[0];
	// Built-in?
	if ( strcmp(command,strdup("exit")) == 0 )
		return EXIT;
	if ( strcmp(command,strdup("cd")) == 0 )
		return CD;
	if ( strcmp(command,strdup("path")) == 0 )
		return PATH;
	// If statement?
	if ( strcmp(command,strdup("if")) == 0 )
		return IF;
	// Redirect?
	for (int i = 0; i < num_tokens; i++)
	{
		char* token = strdup(tokens[i]);
		if( strcmp(token, strdup(">")) == 0 )
			return (i != 0) ? REDIRECT : ERROR;
	}
	// Not any of the above, must be a program
	return PROGRAM;
}


int buildArgs(char* tokens[], int num_tokens, char* args[], int start_index, int final_index)
{	
	if (final_index >= num_tokens-1)
		final_index = num_tokens-1;

	int argc = 0;
	for (int i = 0; i < final_index-start_index+1; i++)
	{
		char* token = strdup(tokens[start_index + i]);
		args[i] = token;
		argc++;
	}
	args[argc] = NULL;

	// return the number of arguments in arg
	return argc;
}


char* findProgPath(char* args[], int argc)
{
// Find program's path
	char* prog_path = malloc( sizeof(char) * MAX_PATH_LENGTH );
	char* prog = strdup(args[0]);
	for( int i=0; i < NUM_PATHS; i++)
	{
		// Build possible program path
		strcpy(prog_path, BIN_PATHS[i]);
		strcat(prog_path, "/");
		strcat(prog_path, prog);

		// Check if that program path exists
		if( access(prog_path, X_OK) == 0 )
			return strdup(prog_path);
		else
			continue;
	}
	// Exhausted all paths, fail
	return NULL;
}


/**
 *  Execute a program with the given args.
*/
int execProg(char* args[], int argc, char* redirect_file)
{
	// Get a valid program path
	char* prog_path = findProgPath(args, argc);
	if (prog_path == NULL)
		return false;
	// Create new process to execute program
	pid_t pid = fork();
	if( pid == 0 )
	{
		// Redirect stdout if needed
		if(redirect_file != NULL)
		{
			int fd = open(redirect_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    		dup2(fd, fileno(stdout));
    		close(fd);
		}
		execv(prog_path, args);
	}
	int status;
	waitpid(pid, &status, 0);
	// Successfully ran program!  We can return now
	int rc = WEXITSTATUS(status);
	return rc;
}


/**
 *  Find the index of the redirect character ">".
 * 
 * 	Param:
 * 		char* tokens[]
 * 		int num_tokens
 * 
 * 	Returns:
 * 		Index of ">", -1 if not found
*/
int findRedirect(char* tokens[], int num_tokens)
{
	for (int i = 0; i < num_tokens; i++)
	{
		char* token = tokens[i];
		if ( strcmp(token, strdup(">")) == 0 )
			return i;
	}
	return -1;
}


char* getRedirect(char* tokens[], int num_tokens)
{
	int redirect_index = findRedirect(tokens, num_tokens);
	// Single redirect file NEEDS to be immediately after redirect
	if (redirect_index + 1 == num_tokens - 1)
		return strdup(tokens[redirect_index+1]);
	return NULL;
}


int evalIfCondition(char* condition_args[], int condition_argc)
{
	// Unpack args
	char* left_operand = malloc( sizeof(char) * MAX_LINE_LENGTH );
	char* str_op = malloc( sizeof(char) * MAX_LINE_LENGTH );
	char* right_operand = malloc( sizeof(char) * MAX_LINE_LENGTH );
	left_operand = strdup(condition_args[0]);
	str_op = strdup(condition_args[1]);
	right_operand = strdup(condition_args[2]);

	// Define types of operators
	typedef enum {
		EQUALS,
		NOT_EQUALS
	} op_t;

	// Translate str_op into the enum type
	op_t op;
	if ( strcmp( str_op, strdup("==") ) == 0 )
		op = EQUALS;
	else if ( strcmp( str_op, strdup("!=") ) == 0 )
		op = NOT_EQUALS;

	// Execute the left operand
	int left_val = executeCommand(&left_operand, 1);
	int right_val = atoi(right_operand);

	switch (op)
	{
		case EQUALS:
			return left_val == right_val;
			break;

		case NOT_EQUALS:
			return left_val != right_val;
			break;
	}
	return 0;
}


/**
 * 	Execute a command.
 * 
 * 	Parameters:
 * 		char* tokens[] - array of tokens
 */
int executeCommand(char* tokens[], int num_tokens)
{
	// Determine what kind of command this is
	COMMAND_T command = determineCommand(tokens, num_tokens);

	// Allocate space for args
	char* args[MAX_NUM_TOKENS];
	int argc = 0;

	// Allocate space for IF args
	char* condition_args[MAX_NUM_TOKENS];
	int condition_argc = 0;
	char* then_args[MAX_NUM_TOKENS];
	int then_argc = 0;

	// Process command
	int status = -1;
	switch (command)
	{
		case EXIT:
			status = builtinExit(tokens, num_tokens);
			break;
		case CD:
			status = builtinCd(tokens, num_tokens);
			break;
		case PATH:
			status = builtinPath(tokens, num_tokens);
			break;
		case PROGRAM:
			argc = buildArgs(tokens, num_tokens, args, 0, num_tokens-1);
			status = execProg(args, argc, NULL);
			break;
		case REDIRECT:
			int redirect_index = findRedirect(tokens, num_tokens);
			char* redirect_file = getRedirect(tokens, num_tokens);
			if (redirect_file == NULL) break;
			argc = buildArgs(tokens, num_tokens, args, 0, redirect_index-1);
			status = execProg(args, argc, redirect_file);
			break;
		case IF:
			condition_argc = buildArgs(tokens, num_tokens, condition_args, 1, 3);
			then_argc = buildArgs(tokens, num_tokens, then_args, 5, num_tokens-2);
			if ( evalIfCondition(condition_args, condition_argc) )
			{
				status = executeCommand(then_args, then_argc);
			}
			break;
		case ERROR:
			break;
	}
	// Error?
	return status;
}


int main(int argc, char *argv[])
{
	// ERROR: Passed more than one file or bad file
	if ( argc >= 3 || argc == 2 &&  access(argv[1], R_OK) != 0)
	{
		error();
		exit(1);
	}

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
		if (num_tokens >= 1)
		{
			int status = executeCommand(tokens, num_tokens);
			if (status != 0)
				error();
		}
			
	}
	return 0;
}

