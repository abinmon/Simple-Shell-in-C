#include "simpleShell.h"
aliases array[ARG_MAX];
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
    const char delimiters[] = " \t\n;&><|";
    char history[ARR_SIZE][ARG_MAX] = {0};
    int cmdNumber = 0;
    int NumberOfAlias;

    while(1) {
        init();

        String c = fgets(buffer, ARG_MAX, stdin);
        String secondArgument = calloc(strlen(buffer)*ARR_SIZE, sizeof(char));
        strcpy(secondArgument,buffer);


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
            String* tokens;
            tokens = getTokens(buffer, delimiters);
            // Store History
            if(buffer[0] != '!') {
                storeHistory(history, &cmdNumber, buffer, tokens);
            }
            checkInput(tokens, buffer, history, &cmdNumber, secondArgument, delimiters, &NumberOfAlias);
        }
    }
}

void checkInput(String* tokens, const char* buffer, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, String secondArgument, const char delimiters[], int* NumberOfAlias) {
    checkAlias(&secondArgument);
    getTokens(secondArgument, delimiters);
    if (strncmp(tokens[0], "getpath", 7) == 0) {
        if(tokens[1] == NULL) {
            getPath();
        }
        else {
            printf("There should be no arguments");
            printf("\n");
        }
    } else if (strncmp(tokens[0], "setpath", 7) == 0) {
        if (tokens[1]) {
            setPath(tokens[1]);
        } else {
            printf("Empty value\n");
        }
    } else if (strcmp(tokens[0], "history") == 0) {
        getFullHistory(history, cmdNumber);
    } else if (buffer[0] == '!') {
        extractHistory(tokens, history, cmdNumber, delimiters);
    }
    else if (strncmp(tokens[0], "alias", 5) == 0) {
        addAlias(tokens, NumberOfAlias);
    }
    else if (strncmp(tokens[0], "unalias", 5) == 0) {
        unalias(tokens, NumberOfAlias);
    }
    else {
        runCommand(tokens);
    }
}

void runCommand(String ls_args[]){
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

void getPath(){
    printf("%s\n", getenv("PATH"));
}

String* getTokens(String cmd, const char delimiters[]) {
    static String tokensList[] = {NULL};
    // Copy variable in temp variable
    String tempStr = calloc(strlen(cmd)+1, sizeof(char));
    strcpy(tempStr, cmd);
    // Divide in tokens
    String token = strtok(tempStr, delimiters);
    int i = 0;
    // walk through other tokens
    while( token != NULL ) {
        tokensList[i] = token;
        token = strtok(NULL, delimiters);
        i++;
    }
    tokensList[i] = NULL;
    return tokensList;
}

void setPath(String newPath) {
    if (checkDirectory(newPath)) {
        setenv("PATH", newPath, 1);
    } else {
        perror(newPath);
    }
}

void storeHistory(char history[ARR_SIZE][ARG_MAX], int *cmdNum, String cmd, String* tokens) {
    if (strcmp(tokens[0], "history") == 0) {
        if (tokens[1]) {
            printf("History command not valid!\n");
            return;
        }
        if (*cmdNum == 0) {
            printf("History is empty!\n");
            return;
        }
    }
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


void getHistory(char history[ARR_SIZE][ARG_MAX], int index, const char delimiters[]) {
    int lessIndex = index;
    char *cmd = history[--lessIndex];
    checkInput(getTokens(cmd, delimiters), cmd, history, &index, NULL, delimiters, NULL);
}

int isNumeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
        return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

void extractHistory(String* tokens, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, const char delimiters[]) {
    if (tokens[1]){
        printf("Too many arguments\n");
        return;
    }
    if (strcmp(tokens[0], "!!") == 0) {
        if (*cmdNumber > 0) {
            getHistory(history, *cmdNumber, delimiters);
        } else {
            printf("History is empty!\n");
        }
    } else {
        String charIndex = NULL;
        // Check if remainder command or normal
        // by checking if command starts with !-
        if (strncmp(tokens[0], "!-", strlen("!-")) == 0 && isNumeric(&tokens[0][2])) {
            // Take command number for remainder
            charIndex = &tokens[0][2];
            getIndexHistory(charIndex, history, cmdNumber, delimiters, true);
        } else if (isNumeric(&tokens[0][1])) {
            // Take command number
            charIndex = &tokens[0][1];
            getIndexHistory(charIndex, history, cmdNumber, delimiters, false);
        } else {
            printf("Enter a valid number! \n");
        }
    }
}

void getIndexHistory(String charIndex, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, const char delimiters[], bool isRemainder) {
    char *ptr;
    // Convert number from char to int
    int index = (int) strtol(charIndex, &ptr, 10);
    if (isRemainder == true) {
        if (*cmdNumber > 20){
            //reset command number to 20
            *cmdNumber = 20;
        }
        // Get remainder
        index--;
        index = *cmdNumber - index;
    }

    // Check if index is in stored history
    if (index > ARR_SIZE) {
        printf("Only 20 history commands allowed!\n");
    } else if (index <= 0 || index > *cmdNumber) {
        printf("History command not found!\n");
    } else {
        getHistory(history, index, delimiters);
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

void addAlias(String *token, int* NumberOfAlias) {
    //If there is more than one command
    char wholeLineCommand[512] = {'\0'};
    int t = 2;
    while(token[t] != NULL){
        strcat(wholeLineCommand, token[t]);
        strcat(wholeLineCommand, " ");
        t++;
    }

    if ((token[1] == NULL)) {
        if (*NumberOfAlias == 0) {
            printf("test 2-");
            printf("There are no current alias");
        } else {
            for (int i = 0; i <= 10; i++) {
                if (array[i].aliasCommand[0] != '\0') {
                    printf("test 3-");
                    printf(" %s ---- %s\n", array[i].aliasName, array[i].aliasCommand);
                }
            }
        }
    }
    else if(token[2] == NULL) {
        printf("test 4-");
        printf("Too Few Arguments");
    }
    else if(token[4] != NULL) {
        printf("test 5-");
        printf("Too many Arguments");
    }
    else {
        //Null Terminating the whitespace
        int len = strlen(wholeLineCommand);
        wholeLineCommand[len - 1] = '\0';
        if(*NumberOfAlias >= 10){
            printf("No more space for aliases\n");
        }else {

            //Look for duplicate aliases
            for (int i = 0; i <= 10; i++) {
                if (strcmp(array[i].aliasName, token[1]) == 0) {
                    printf("test 6");
                    printf("Overwriting alias %s\n", token[1]);
                    strcpy(array[i].aliasName, token[1]);
                    strcpy(array[i].aliasCommand, wholeLineCommand);
                    return;
                }
            //Finding empty position
                if (strcmp(array[i].aliasName, "") == 0) {
                    strcpy(array[i].aliasName, token[1]);
                    strcpy(array[i].aliasCommand, wholeLineCommand);
                    *NumberOfAlias = *NumberOfAlias +1;
                    printf("%d\n", *NumberOfAlias);
                    printf("New Alias %s -- %s\n", token[1], wholeLineCommand);
                    return;
                }
            }
        }
        if(*NumberOfAlias > 10){
            printf("No more space for aliases\n");
        }
    }

}

void unalias(String * token, int *NumberOfAliases){
    if (token[1] == NULL){
        printf("Not enough arguments");
        return;
    }
    if (token[2] != NULL ){
        printf("Too Many Arguments");
    }

    for (int i = 0; i <= 10; i++) {
        if (strcmp(array[i].aliasName, token[1]) == 0) {
            strcpy(array[i].aliasName, "");
            strcpy(array[i].aliasCommand, "");
            strcpy(array[i].aliasName, array[i+1].aliasName);
            strcpy(array[i].aliasCommand,array[i+1].aliasCommand);
            strcpy(array[i+1].aliasName, "");
            strcpy(array[i+1].aliasCommand,"");
            *NumberOfAliases = *NumberOfAliases -1;
            printf("test 11");
            printf("Alias Removed %s\n", token[1]);
            return;
        }

    }


}

void checkAlias(String *input) {
    char* token;
    char line[512] = { '\0' };
    //get command
    token = strtok(*input, " \t;<>|\n&");
    //look for an alias and get the alias command if one is found
    for(int j = 0; j <= 10; j++) {
        if(token != NULL && array[j].aliasName != NULL && (strcmp(token, array[j].aliasName) == 0)) {
            token = array[j].aliasCommand;
        }
    }
    //Building the command
    if (token != NULL) {
        strcpy(line, token);
    }
    //get rest of original line after the possible alias
    token = strtok(NULL, "");
    if(token != NULL) {
        strcat(line, " ");
        strcat(line, token);
    }

    strcpy(*input, line);
}
