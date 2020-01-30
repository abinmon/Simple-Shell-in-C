#include "simpleShell.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

void init() {
	char* username = getenv("USERNAME");
	printf("\nuser@%s", username);
	printf("\n");
	printf("$ ");
}

void readInput(char* commands) {
    gets(commands);
	char *cmd;

    cmd = strtok(commands, " ");
    while( cmd != NULL ) {
        printf("%s\n", cmd);
        cmd = strtok(NULL, " ");
    }
}
