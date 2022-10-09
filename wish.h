
#ifndef WISH_H
#define WISH_H

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Global Constants
#define DEBUG true
const size_t MAX_LINE_LENGTH = 256;
const bool INTERACTIVE = true;

// Global variables
char* BATCH_FILE = NULL;

// Function headers
void prompt(void);
char* getCommand(void);


#endif

