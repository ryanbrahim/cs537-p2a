
#ifndef WISH_H
#define WISH_H

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

// Global Constants
#define DEBUG true
#define MAX_PATH_LENGTH 512
#define MAX_PATHS 256
const size_t MAX_LINE_LENGTH = 512;
const int MAX_NUM_TOKENS = 512;
const bool INTERACTIVE = true;

// Global variables
FILE* INPUT_FILE;
int NUM_PATHS;
char** BIN_PATHS;

// Function headers
void prompt(void);
int getTokens(char* tokens[]);
void executeCommand(char* tokens[], int num_tokens);
void error();

// Builtins
void cd(char* tokens[], int num_tokens);
void path(char* tokens[], int num_tokens);

#endif

