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
void getFullHistory(char history[ARR_SIZE][ARG_MAX], const int *size);
void getHistory(char history[ARR_SIZE][ARG_MAX], int index, const char delimiters[]);
void extractHistory(String* tokens, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, const char delimiters[]);
void getIndexHistory(String charIndex, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, const char delimiters[], bool isRemainder);
void checkInput(String* tokens, const char* buffer, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, String secondArgument, const char delimiters[], int* NumberOfAlias);

void unalias(String * token, int *NumberOfAliases);
void addAlias(String *token, int* NumberOfAlias);
void checkAlias(String *input);