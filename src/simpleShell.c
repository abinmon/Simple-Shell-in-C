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

void readInput() {
    char *cmd;
    char *buffer = malloc(sizeof(char)*ARG_MAX);

    while(1) {
        init();
        // Check for exit
        if (fgets(buffer, ARG_MAX, stdin) == NULL || strncmp(buffer, "exit", 4) == 0) {
            break;
        }

        fflush(stdin);

        // Check if empty string passed
        if (buffer[0] != '\n') {
            // Divide string in tokens
            cmd = strtok(buffer, " \n");
            // Print indentation
            while( cmd != NULL ) {
                printf("\"%s\"\n", cmd);
                cmd = strtok(NULL, " \n");
            }
        }
    }
}