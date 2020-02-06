#include "simpleShell.h"

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
        if (c == NULL ||  (strcmp(buffer, "exit") > 0 && strlen(c) == 5)) {
            printf("%s\n", oldPath);
            break;
        }

        fflush(stdin);

        char *firstToken = getToken(buffer, 0);
        if (strncmp(firstToken, "getpath", 7) == 0) {
            printf("%s\n", getPath());
        } else if (strncmp(firstToken, "setpath", 7) == 0) {
            setPath(getToken(buffer, 1));
        } else {
            system(buffer);
        }

    }
}

char * getPath() {
    return getenv("PATH");
}

char * getToken(char * cmd, int index) {
    char *tokensList[50];
    // Copy variable in temp variable
    char* tempStr = calloc(strlen(cmd)+1, sizeof(char));
    strcpy(tempStr, cmd);
    // Divide in tokens
    char *token = strtok(tempStr, " \n");
    int i = 0;
    // walk through other tokens
    while( token != NULL ) {
        tokensList[i] = token;
        token = strtok(NULL, " \n");
        i++;
    }
    fflush(stdin);
    return tokensList[index];
}

void setPath(char* newPath) {
    if (checkDirectory(newPath) == 1) {
        setenv("PATH", newPath, 1);
//        printf("New Path: %s\n", getPath());
    } else {
        printf("%s\n", "No such file or directory");
    }
}

int checkDirectory(char* s) {
    DIR* dir = opendir(s);
    if (dir) {
        // Directory or file exists
        closedir(dir);
        return 1;
    } else if (ENOENT == errno) {
        // Directory or file does not exist
        return 0;
    } else {
        // opendir() failed for some other reason
        return -1;
    }
}