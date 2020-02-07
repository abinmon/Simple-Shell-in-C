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
            printf("%s\n", getenv("PATH"));
            break;
        }

        fflush(stdin);

        char **tokens;
        tokens = getTokens(buffer);
        if (strncmp(tokens[0], "getpath", 7) == 0) {
            printf("%s\n", getPath());
        } else if (strncmp(tokens[0], "setpath", 7) == 0) {
            if (tokens[1]) {
                setPath(tokens[1]);
            } else {
                printf("%s\n", "No such file or directory");
            }
        } else {
            runCommand(tokens);
        }

    }
}

void runCommand(char *ls_args[]) {
    pid_t c_pid, pid;
    int status;
    c_pid = fork();

    if (c_pid == -1) {
        perror("fork failed");
        _exit(1);
    }
    if (c_pid == 0) {
        execvp(ls_args[0], ls_args);
        perror("execv failed");
    } else if (c_pid > 0) {
        if ( (pid = wait(&status)) < 0) {
            perror("wait failed");
            _exit(1);
        }
    }
}

char * getPath() {
    return getenv("PATH");
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
    if (checkDirectory(newPath) == 1) {
        setenv("PATH", newPath, 1);
        printf("%s", getPath());
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