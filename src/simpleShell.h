/* Header File */
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <errno.h>
#define ARG_MAX 512

void init();
char ** getTokens(char * cmd);
void readInput(char* currPath);
int checkDirectory(char* s);
char * getPath();
void setPath(char* buffer);
void runCommand(char *tokens[]);