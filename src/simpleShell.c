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
        char *c = fgets(buffer, ARG_MAX, stdin);

        // Check for exit
        if (c == NULL || strncmp(buffer, "exit", 4) == 0) {
            break;
        }

        // Check if empty string passed
        if (buffer[0] != '\n') {
            // Print indentation
            printf("  ");
            // Divide string in tokens
            cmd = strtok(buffer, " ");
            while( cmd != NULL ) {
                printf("%s ", cmd);
                cmd = strtok(NULL, " ");
            }
        }
    }
}