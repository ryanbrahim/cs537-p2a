
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

typedef enum {
    EXIT,
    CD,
    PATH,
    PROGRAM,
    REDIRECT,
    IF,
    ERROR
} COMMAND_T;

// Global variables
FILE* INPUT_FILE;
int NUM_PATHS;
char** BIN_PATHS;

// Helper functions
void prompt(void);
int getTokens(char* tokens[]);
void error();
bool handleRedirect(char* tokens[], int num_tokens, int redirect_index);
COMMAND_T determineCommand(char* tokens[], int num_tokens);
int buildArgs(char* tokens[], int num_tokens, char* args[], int start_index, int final_index);
int findRedirect(char* tokens[], int num_tokens);
char* getRedirect(char* tokens[], int num_tokens);
char* findProgPath(char* args[], int argc);

// Command handlers
void executeCommand(char* tokens[], int num_tokens);
bool execProg(char* tokens[], int num_tokens, char* redirect_file);

// Builtins
bool builtinExit(char* tokens[], int num_tokens);
bool builtinCd(char* tokens[], int num_tokens);
bool builtinPath(char* tokens[], int num_tokens);

#endif

