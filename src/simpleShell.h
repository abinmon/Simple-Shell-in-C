/* Header File */
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#define ARG_MAX 512

void init();
char * getToken(char * cmd, int index);
void readInput(char* currPath);
int checkDirectory(char* s);
char * getPath();
void setPath(char* buffer);