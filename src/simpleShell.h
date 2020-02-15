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
#define MAX_ALIAS 10
#define ERR_ARG_MAX "Too many arguments!\n"

struct alias {
    char aliasName[ARG_MAX];
    char aliasCommand[ARG_MAX];
};
typedef struct alias aliases;



typedef char* String;
typedef enum {true, false} bool;
void init();
void readInput(String currPath);
int checkDirectory(String s);
void getPath();
String* getTokens(String cmd, const char delimiters[]);
void setPath(String newPath);
void runCommand(String tokens[]);
void chwDir();
void storeHistory(char history[ARR_SIZE][ARG_MAX], int *cmdNum, String cmd, String* tokens);
void getFullHistory(char history[ARR_SIZE][ARG_MAX]);
void getHistory(char history[ARR_SIZE][ARG_MAX], int index, const char delimiters[] , int *num, String secondArgument);
void extractHistory(String* tokens, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, const char delimiters[], int *num, String arg);
void getIndexHistory(String charIndex, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, const char delimiters[], bool isRemainder, int *num, String arg );
void checkInput(String* tokens, const char* buffer, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, const char delimiters[], int *Num, String arg);

void unalias(String * token, int *NumberOfAliases);
void addAlias(String *token, int* NumberOfAlias);
void checkAlias(String *input);
aliases array[ARG_MAX];