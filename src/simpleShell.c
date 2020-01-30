#include "simpleShell.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#define ARG_MAX 512

void init() {
	char* username = getenv("USERNAME");
	printf("\nuser@%s", username);
	printf("\n");
    printf("$ ");
}

char readInput() {
    char *cmd;
    char *commands = malloc(sizeof(char)*ARG_MAX);

    while(1) {
        init();
        commands = fgets(commands, ARG_MAX, stdin);

        if (commands == EOF || commands == '\n' || strncmp(commands, "exit", 4) == 0) {
            return 0;
        }

        cmd = strtok(commands, " ");
        while( cmd != NULL ) {
            printf("%s ", cmd);
            cmd = strtok(NULL, " ");
        }
    }
}