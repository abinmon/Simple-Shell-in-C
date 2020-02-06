#include "simpleShell.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#define ARG_MAX 512

void init() {
	char* username = getenv("USER");
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

        // Remove leading spaces when exit is entered
        while (isspace(*buffer))
        {
            buffer = buffer + 1;
        }
        // Check for exit
        if (c == NULL || strcmp(buffer, "exit") == 0) {
            break;
        }

        fflush(stdin);

        // Check if empty string passed
        if (buffer[0] != '\n') {
            // Divide string in tokens
            cmd = strtok(buffer, " \n\t;><&|");
            // Print indentation
            while( cmd != NULL ) {
                printf("\"%s\"\n", cmd);
                cmd = strtok(NULL, " \n\t;><&|");
            }
        }
    }
}