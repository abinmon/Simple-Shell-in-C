#include "simpleShell.h"

/**
 * Init function for shell terminal starting point
 *
 */
void init() {
    String username = getenv("USER");
    printf("\nuser@%s", username);
    printf("\n");
    printf("$ ");
}

/**
 * Change working directory to HOME environment
 *
 */
void chwDir() {
    char cwd[ARG_MAX];
    printf("Old working dir: %s\n", getcwd(cwd, sizeof(cwd)));
    // Set the current working directory to home
    chdir(getenv("HOME"));
    printf("Current working dir: %s\n", getcwd(cwd, sizeof(cwd)));
}

/**
 * Read User input
 *
 * @param oldPath
 */
void readInput(String oldPath) {
    String buffer = malloc(sizeof(char)*ARG_MAX);
    const char delimiters[] = " \t\n;&><|";
    char history[ARR_SIZE][ARG_MAX] = {0};
    int cmdNumber = 0;
    int Num =0 ;
    String secondArgument = calloc(strlen(buffer)+1, sizeof(char));



    while(1) {
        init();

        String c = fgets(buffer, ARG_MAX, stdin);
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
            checkAlias(&secondArgument);
            getTokens(secondArgument, " \t;<>|\n&");
            checkInput(tokens, buffer, history, &cmdNumber, delimiters, &Num, secondArgument);
        }
    }
}

/**
 * Check parsed input for built-in or external commands
 *
 * @param tokens
 * @param buffer
 * @param history
 * @param cmdNumber
 * @param delimiters
 */
void checkInput(String* tokens, const char* buffer, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, const char delimiters[], int *num, String secondArgument ) {
    if (strncmp(tokens[0], "getpath", 7) == 0) {
        if (tokens[1] == NULL)
        {
            getPath();
        }
        else {
            printf(ERR_ARG_MAX);
        }
    } else if (strncmp(tokens[0], "setpath", 7) == 0) {
        if (tokens[1] && (tokens[2] == NULL))
        {
            setPath(tokens[1]);
        }
        else if (tokens[1] == NULL)
        {
            printf("Empty value\n");
        }
        else {
            printf(ERR_ARG_MAX);
        }
    } else if ((strncmp(tokens[0], "cd", 2) == 0)) {
        if (tokens[1] != NULL && tokens[2] != NULL) {
            printf(ERR_ARG_MAX);
        } else if (tokens[1] && (checkDirectory(&tokens[0][3]) > 0)) {
            chdir(tokens[1]);
        } else if (*tokens[1] == '\0') {
            chdir(getenv("HOME"));
        } else {
            perror(tokens[1]);
        }
        char *cwd = getcwd(NULL, 0);
        free(cwd);
    } else if (strcmp(tokens[0], "history") == 0) {
        if (tokens[1] == NULL)
        {
            getFullHistory(history);
        }
    } else if (buffer[0] == '!') {
        extractHistory(tokens, history, cmdNumber, delimiters, num,secondArgument);
    }
    else if (strncmp(tokens[0], "alias", 5) == 0) {

        addAlias(tokens, num);
    }
    else if (strncmp(tokens[0], "unalias", 5) == 0) {
        unalias(tokens, num);
    }

    else {
        runCommand(tokens);
    }
}

/**
 * Run external commands
 *
 * @param ls_args
 */
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

/**
 * Return the PATH environment
 *
 */
void getPath() {
    printf("%s\n", getenv("PATH"));
}

/**
 * Get String tokens with specified delimiters
 *
 * @param cmd
 * @param delimiters
 * @return
 */
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

/**
 * Set the environment PATH
 *
 * @param newPath
 */
void setPath(String newPath) {
    if (checkDirectory(newPath)) {
        setenv("PATH", newPath, 1);
    } else {
        perror(newPath);
    }
}

/**
 * Store history with full command String
 *
 * @param history
 * @param cmdNum
 * @param cmd
 * @param tokens
 */
void storeHistory(char history[ARR_SIZE][ARG_MAX], int *cmdNum, String cmd, String* tokens) {
    if (strcmp(tokens[0], "history") == 0) {
        if (tokens[1]) {
            printf("History command not valid!\n");

            if (*cmdNum >= ARR_SIZE) {
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

            return;
        }
        if (*cmdNum == 0) {
            printf("History is empty!\n");
            return;
        }
    }
    if (*cmdNum >= ARR_SIZE) {
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

/**
 * Print full history of commands
 *
 * @param history
 * @param size
 */
void getFullHistory(char history[ARR_SIZE][ARG_MAX]) {
    for(int i = 0; i < ARR_SIZE; i++) {
        if (strcmp(history[i], "") > 0) {
            printf("\n%d. %s", i + 1, history[i]);
        }
    }

}

/**
 * Run history command selected through index
 *
 * @param history
 * @param index
 * @param delimiters
 */
void getHistory(char history[ARR_SIZE][ARG_MAX], int index, const char delimiters[] , int *num, String secondArgument) {
    int lessIndex = index;
    char *cmd = history[--lessIndex];
    checkAlias(&cmd);
    checkInput(getTokens(cmd, delimiters), cmd, history, &index, delimiters, num, secondArgument);
}

/**
 * Check if string passed is numeric
 *
 * @param s
 * @return
 */
int isNumeric (const char * s)
{
    if (s == NULL || *s == '\0' || isspace(*s))
        return 0;
    char * p;
    strtod (s, &p);
    return *p == '\0';
}

/**
 * Extract history by looking at which command to use
 *
 * @param tokens
 * @param history
 * @param cmdNumber
 * @param delimiters
 */
void extractHistory(String* tokens, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, const char delimiters[], int *num, String arg) {
    if (tokens [1]) {
        printf(ERR_ARG_MAX);
        return;
    }
    if (strcmp(tokens[0], "!!") == 0) {
        if (*cmdNumber > 0) {
            getHistory(history, *cmdNumber, delimiters, num, arg);
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
            getIndexHistory(charIndex, history, cmdNumber, delimiters, true, num, arg);
        } else if (isNumeric(&tokens[0][1])) {
            // Take command number
            charIndex = &tokens[0][1];
            getIndexHistory(charIndex, history, cmdNumber, delimiters, false, num, arg);
        } else {
            printf("Enter a valid number! \n");
        }
    }
}

/**
 * Get the index of specific history command
 *
 * @param charIndex
 * @param history
 * @param cmdNumber
 * @param delimiters
 * @param isRemainder
 */
void getIndexHistory(String charIndex, char history[ARR_SIZE][ARG_MAX], int *cmdNumber, const char delimiters[], bool isRemainder, int *num, String arg ) {
    char *ptr;
    // Convert number from char to int
    int index = (int) strtol(charIndex, &ptr, 10);
    if (isRemainder == true) {
        if (*cmdNumber > 20) {
            // Reset commands number to 20 (MAXIMUM)
            *cmdNumber = 20;
        }
        // Get remainder
        // Shift index to the left
        // because of starting with index 1
        index--;
        index = *cmdNumber - index;
    }

    // Check if index is in stored history
    if (index > ARR_SIZE) {
        printf("Only 20 history commands allowed!\n");
    } else if (index <= 0 || index > *cmdNumber) {
        printf("History command not found!\n");
    } else {
        getHistory(history, index, delimiters, num, arg);
    }

}

/**
 * Check if directory exists
 *
 * @param s:path
 * @return
 */
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
    //If there is more than one commandf
    char wholeLineCommand[512] = {'\0'};
    int t = 2;
    while(token[t] != NULL){
        strcat(wholeLineCommand, token[t]);
        strcat(wholeLineCommand, " ");
        t++;
    }

    if ((token[1] == NULL)) {
        if (*NumberOfAlias == 0) {
            printf("There are no current alias\n");
        } else {
            for (int i = 0; i <= 10; i++) {
                if (array[i].aliasCommand[0] != '\0') {

                    printf("\n %s ---- %s\n", array[i].aliasName, array[i].aliasCommand);
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
    else {

        //Null Terminating the whitespace
        int len = strlen(wholeLineCommand);
        wholeLineCommand[len - 1] = '\0';

            //Look for duplicate aliases
            for (int i = 0; i <= 11; i++) {
                if (strcmp(array[i].aliasName, token[1]) == 0) {
                    printf("Overwriting alias %s\n", token[1]);
                    strcpy(array[i].aliasName, token[1]);
                    strcpy(array[i].aliasCommand, wholeLineCommand);
                    return;
                }
                //Finding empty position
                if (strcmp(array[i].aliasName, "") == 0) {
                    if(*NumberOfAlias >= 10){
                        printf("Alias list full");
                        return;
                    }
                    strcpy(array[i].aliasName, token[1]);
                    strcpy(array[i].aliasCommand, wholeLineCommand);
                    *NumberOfAlias = *NumberOfAlias +1;
                    printf("New Alias %s -- %s\n", token[1], wholeLineCommand);
                    return;
                }
            }

    }

}

void unalias(String * token, int *NumberOfAliases){
    int set = 0;
    if (token[1] == NULL){
        printf("Not enough arguments\n");
        return;
    }
    if (token[2] != NULL ){
        printf("Too Many Arguments\n");
    }
    if(*NumberOfAliases <= 0){
        printf("Alias list is empty!\n");
        return;
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
            printf("Alias Removed %s\n", token[1]);
            return;
        }
        else {
            set = 1;
        }


    }
    if(set ==1){
        printf("There are no such aliases in the list\n");
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

