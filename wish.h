
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
int getCommand(char* tokens[]);


#endif

