#include "simpleShell.h"

void init() {
	char* username = getenv("USER");
    printf("\nuser@%s", username);
	printf("\n");
    printf("$ ");
}

void chwDir() {
    char cwd[ARG_MAX];
    printf("Old working dir: %s\n", getcwd(cwd, sizeof(cwd)));
    // Set the current working directory to home
    chdir(getenv("HOME"));
    printf("Current working dir: %s\n", getcwd(cwd, sizeof(cwd)));
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
            // Restore old Path
            setenv("PATH", oldPath, 1);
            getPath();
            break;
        }

        fflush(stdin);

        if (buffer[0] != '\0') {
            char **tokens;
            tokens = getTokens(buffer);
            if (strncmp(tokens[0], "getpath", 7) == 0) {
                getPath();
            } else if (strncmp(tokens[0], "setpath", 7) == 0) {
                setPath(tokens[1]);
            } else {
                runCommand(tokens);
            }
        }

    }
}

void runCommand(char *ls_args[]) {
    pid_t c_pid;
    c_pid = fork();

    if (c_pid < 0) {
        perror("fork failed");
        exit(1);
    }
    if (c_pid == 0) {
        if (execvp(ls_args[0], ls_args) < 0) {
            perror(ls_args[0]);
            exit(1);
        }
        exit(0);
    } else {
        wait(NULL);
    }
}

void getPath() {
    printf("%s\n", getenv("PATH"));
}

char** getTokens(char * cmd) {
    static char *tokensList[] = {NULL};
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
    tokensList[i] = NULL;
    return tokensList;
}

void setPath(char* newPath) {
    if (newPath) {
        setenv("PATH", newPath, 1);
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