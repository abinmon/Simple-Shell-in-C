#include "simpleShell.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#define ARG_MAX 512

void init() {
	char* username = getenv("USER");
    char cwd[ARG_MAX];
    // Set the current working directory to home
    chdir(getenv("HOME"));
    printf("Current working dir: %s\n", getcwd(cwd, sizeof(cwd)));
    printf("\nuser@%s", username);
	printf("\n");
    printf("$ ");
}


void readInput(char * oldPath) {
    char *buffer = malloc(sizeof(char)*ARG_MAX);

    while(1) {
        init();

        char *c = fgets(buffer, ARG_MAX, stdin);

        // Remove leading spaces when exit is entered
        while (isspace(*buffer))
        {
            ++buffer;
        }
        // Check for exit
        if (c == NULL || strcmp(buffer, "exit") == 0) {
            printf("%s\n", oldPath);
            break;
        }

        fflush(stdin);

        if (strncmp(buffer, "getpath", 7) == 0) {
            printf("%s", getPath());
        } else if (strncmp(buffer, "setpath", 7) == 0) {
            setPath(buffer);
        } else {
            system(buffer);
        }

    }
}

char * getPath() {
    return getenv("PATH");
}

void setPath(char* newPath) {
    char *cmd;
    // Get second token and set the path
    strtok(newPath, " \n");
    cmd = strtok(NULL, " \n");
    if (checkDirectory(cmd) == 1) {
        setenv("PATH", cmd, 1);
        printf("New Path: %s\n", getPath());
    } else {
        printf("%s", "No such file or directory\n");
    }
}

int checkDirectory(char* s) {
    DIR* dir = opendir(s);
    if (dir) {
        // Directory exists
        closedir(dir);
        return 1;
    } else if (ENOENT == errno) {
        // Directory does not exist
        return 0;
    } else {
        // opendir() failed for some other reason
        return -1;
    }
}