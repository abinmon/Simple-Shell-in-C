/* Header File */
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#define ARG_MAX 512
#define ARR_SIZE 20

typedef char* String;
typedef enum {true, false} bool;
void init();
String* getTokens(String cmd);
void readInput(String currPath);
int checkDirectory(String s);
void getPath();
void setPath(String newPath);
void runCommand(String tokens[]);
void chwDir();
void storeHistory(char history[ARR_SIZE][ARG_MAX], int *cmdNum, String cmd);
void getFullHistory(char history[ARR_SIZE][ARG_MAX], const int *size);
void getHistory(char history[ARR_SIZE][ARG_MAX], int index);
void extractHistory(String* tokens, char history[ARR_SIZE][ARG_MAX], const int *cmdNumber);
void checkInput(String* tokens,  const char* buffer, char history[ARR_SIZE][ARG_MAX], const int *cmdNumber);