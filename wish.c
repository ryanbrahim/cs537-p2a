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
 * 	Returns:
 * 		char* command string
 **/
char* getCommand(void)
{
	// Display shell prompt
	prompt();
	// Read user input from CLI
	size_t size = 0;
	char* line = NULL;
	getline(&line, &size, (INTERACTIVE) ? stdin : BATCH_FILE);
	return line;
}




int main()
{
	if (DEBUG)
		setvbuf(stdout, NULL, _IONBF, 0); // turn off buffering for stdout

	// Main shell loop
	while (1)
	{
		// Get next command
		char* line = getCommand();

		// Display that command
		printf("%s", line);
	}
	return 0;
}

