#include "simpleShell.h"

void init() {
	String username = getenv("USER");
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


void readInput(String oldPath) {
    String buffer = malloc(sizeof(char)*ARG_MAX);
    char history[ARR_SIZE][ARG_MAX] = {0};
    int cmdNumber = 0;

    while(1) {
        init();

        String c = fgets(buffer, ARG_MAX, stdin);

        // Remove leading spaces when exit is entered
        while (isspace(*buffer))
        {
            ++buffer;
        }
        // Check for exit
        if (c == NULL ||  strcmp(buffer, "exit\n") == 0) {
            // Restore old Path
            setenv("PATH", oldPath, 1);
            getPath();
            break;
        }

        fflush(stdin);

        if (buffer[0] != '\0') {

            // Store History
            if(buffer[0] != '!') {
                storeHistory(history, &cmdNumber, buffer);
            }

            String* tokens;
            tokens = getTokens(buffer);
            checkInput(tokens, buffer, history, &cmdNumber);
        }
    }
}

void checkInput(String* tokens, const char* buffer, char history[ARR_SIZE][ARG_MAX], const int *cmdNumber) {
    if (strncmp(tokens[0], "getpath", 7) == 0) {
        getPath();
    } else if (strncmp(tokens[0], "setpath", 7) == 0) {
        setPath(tokens[1]);
    } else if (strncmp(tokens[0], "history", 7) == 0) {
        getFullHistory(history, &(*cmdNumber));
    } else if (buffer[0] == '!') {
        extractHistory(tokens, history, &(*cmdNumber));
    }
    else {
        runCommand(tokens);
    }
}

void runCommand(String ls_args[]) {
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

String* getTokens(String cmd) {
    static String tokensList[] = {NULL};
    // Copy variable in temp variable
    String tempStr = calloc(strlen(cmd)+1, sizeof(char));
    strcpy(tempStr, cmd);
    // Divide in tokens
    String token = strtok(tempStr, " \n");
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

void setPath(String newPath) {
    if (newPath) {
        setenv("PATH", newPath, 1);
    } else {
        printf("%s\n", "No such file or directory");
    }
}

void storeHistory(char history[ARR_SIZE][ARG_MAX], int *cmdNum, String cmd) {
    if (*cmdNum >= ARR_SIZE) {
        printf("%d\n", *cmdNum);
        // Shift elements to the left by one
        // to add the new one
        for (int i = 0; i < ARR_SIZE-1; i++) {
            strcpy(history[i], history[i+1]);
        }
        strcpy(history[ARR_SIZE-1], cmd);
    } else {
        strcpy(history[*cmdNum], cmd);
    }
    *cmdNum = *cmdNum + 1;
}

void getFullHistory(char history[ARR_SIZE][ARG_MAX], const int *size) {
    for(int i = 0; i < ARR_SIZE && i < *size; i++) {
        printf("%d. %s", i + 1, history[i]);
    }
}

void getHistory(char history[ARR_SIZE][ARG_MAX], int index) {
    if (index > 0 && index <= ARR_SIZE) {
        int lessIndex = index;
        char *cmd = history[--lessIndex];
        checkInput(getTokens(cmd), cmd, history, &index);
    } else {
        printf("History is empty!\n");
    }
}

void extractHistory(String* tokens, char history[ARR_SIZE][ARG_MAX], const int *cmdNumber) {
    if (strncmp(tokens[0], "!!", 2) == 0) {
        // Get last command history
        getHistory(history, *cmdNumber);
    }
    else {
        String charIndex = NULL;
        bool isRemainder = false;
        char *ptr;
        // Check if remainder command or normal
        // by checking if command starts with !-
        if (strncmp(tokens[0], "!-", strlen("!-")) == 0) {
            // Take command number for remainder
            charIndex = &tokens[0][2];
            isRemainder = true;
        } else {
            // Take command number
            charIndex = &tokens[0][1];
        }
        // Convert number from char to int
        int index = (int) strtol(charIndex, &ptr, 10);
        if (isRemainder == true) {
            // Get remainder
            index = *cmdNumber - index;
        }

        // Check if index is in stored history
        if (index <= *cmdNumber) {
            getHistory(history, index);
        } else {
            printf("History is empty!\n");
        }
    }
}

int checkDirectory(String s) {
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