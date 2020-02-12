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
    char history[ARR_SIZE][ARG_MAX] = {0};
    int cmdNumber = 0;

    while(1) {
        init();

        String c = fgets(buffer, ARG_MAX, stdin);
        String secondArgument = calloc(strlen(buffer)+1, sizeof(char));
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

            // Store History
            if(buffer[0] != '!') {
                storeHistory(history, &cmdNumber, buffer);
            }

            String* tokens;
            tokens = getTokens(buffer);
            checkInput(tokens, buffer, history, &cmdNumber, secondArgument);
        }
    }
}

void checkInput(String* tokens, const char* buffer, char history[ARR_SIZE][ARG_MAX], const int *cmdNumber, String secondArgument) {
    checkAlias(&secondArgument);
    getTokens(secondArgument);
    if (strncmp(tokens[0], "getpath", 7) == 0) {
        getPath();
    } else if (strncmp(tokens[0], "setpath", 7) == 0) {
        setPath(tokens[1]);
    } else if (strncmp(tokens[0], "history", 7) == 0) {
        getFullHistory(history, &(*cmdNumber));
    } else if (buffer[0] == '!') {
        extractHistory(tokens, history, &(*cmdNumber));
    }
    else if (strncmp(tokens[0], "alias", 5) == 0) {
        addAlias(tokens);
    }
    else if (strncmp(tokens[0], "unalias", 5) == 0) {
        unalias(tokens);
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

const char * getFullHistory(char history[ARR_SIZE][ARG_MAX], const int *size) {
    for(int i = 0; i < ARR_SIZE && i < *size; i++) {
        printf("%d. %s", i + 1, history[i]);
    }
}

void getHistory(char history[ARR_SIZE][ARG_MAX], int index) {
    String c = "";
    if (index > 0 && index <= ARR_SIZE) {
        int lessIndex = index;
        char *cmd = history[--lessIndex];
        checkInput(getTokens(cmd), cmd, history, &index,c);
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

void addAlias(String *token) {

    int NumberOfAliases = 0;
    //If there is more than one command
    char wholeLineCommand[512] = {'\0'};
    int i = 2;
    while(token[i] != NULL){
        strcat(wholeLineCommand, token[i]);
        strcat(wholeLineCommand, " ");
        i++;
    }
    for (int count = 0; count < 10; count++) {
        if (array[count].aliasCommand[0] != '\0') {
            NumberOfAliases++;
        }
    }
    if ((token[1] == NULL)) {
        if (NumberOfAliases == 0) {
            printf("There are no current alias");
        } else {
            for (int i = 0; i < 10; i++) {
                if (array[i].aliasCommand[0] != '\0') {
                    printf("%d. %s ---- %s\n",i , array[i].aliasName, array[i].aliasCommand);
                }
            }
        }
    }
    else if(token[2] == NULL) {
        printf("Too Few Arguments");
    }
    else if(token[4] != NULL) {
        printf("Too many Arguments");
    }
    else if(NumberOfAliases <= 11) {

        //Null Terminating the whitespace
        int len = strlen(wholeLineCommand);
        wholeLineCommand[len - 1] = '\0';

        //Look for duplicate aliases
        for (int i = 0; i < 10; i++) {
            if (strcmp(array[i].aliasName, token[1]) == 0) {
                printf("Overwriting alias %s\n", token[1]);
                strcpy(array[i].aliasName, token[1]);
                strcpy(array[i].aliasCommand, wholeLineCommand);

                return;
            }
        }

        //Finding empty position
        for (int i = 0; i < 10; i++) {
            if (strcmp(array[i].aliasName, "") == 0) {
                strcpy(array[i].aliasName, token[1]);
                strcpy(array[i].aliasCommand, wholeLineCommand);
                printf("New Alias %s -- %s\n", token[1], wholeLineCommand);

                return;
            }
        }
    }
    else if (NumberOfAliases > 10){
        printf("No more space for aliases\n");
    }
}

void unalias(String * token){
    int set = 0;
    int count = 0;
    if (token[1] == NULL){
        printf("Not enough arguments");
        return;
    }
    if (token[2] != NULL ){
        printf("Too Many Arguments");
    }

    for (int i = 0; i < 10; i++) {
        if (strcmp(array[i].aliasName, token[1]) == 0) {
            strcpy(array[i].aliasName, "");
            strcpy(array[i].aliasCommand, "");
            strcpy(array[i].aliasName, array[i+1].aliasName);
            strcpy(array[i].aliasCommand,array[i+1].aliasCommand);
            strcpy(array[i+1].aliasName, "");
            strcpy(array[i+1].aliasCommand,"");
            printf("Alias Removed %s\n", token[1]);
            return;
        }
        else if(strcmp(array[0].aliasName, "") == 0){
            set = 1;
            break;
        }
        else {
            count = 1;
        }

    }
//     if(set == 0 && count == 1) {
//
//       //  printf("There is no such alias");
//     } else
        if(set == 1 && count == 0){
        printf("Alias is empty");

    }

}

void checkAlias(String *input) {
    char* token;
    char line[512] = { '\0' };
    //get command
    token = strtok(*input, " \t;<>|\n&");
    //look for an alias and get the alias command if one is foun
    for(int j = 0; j < 10; j++) {
        if(token != NULL && array[j].aliasName != NULL && (strcmp(token, array[j].aliasName) == 0)) {
            token = array[j].aliasCommand;
        }
    }
    // start building the actual line that must be executed
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