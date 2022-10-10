
#ifndef WISH_H
#define WISH_H

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Global Constants
#define DEBUG true
const size_t MAX_LINE_LENGTH = 512;
const int MAX_NUM_TOKENS = 512;
const bool INTERACTIVE = true;

// Global variables
FILE* INPUT_FILE;

// Function headers
void prompt(void);
int getTokens(char* tokens[]);
bool parseCommand(char* tokens[], int num_tokens, char command[], char* argv[]);
void executeCommand(char command[], char* args[]);

#endif

