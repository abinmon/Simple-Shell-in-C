/* Main File */
#include <stdio.h>
#include <stdlib.h>
#include "simpleShell.h"
#include "string.h"
#define ARG_MAX 512

int main(void) {
	char commands[ARG_MAX];
	init();
	do{
	    printf("$ ");
  	    readInput(fgets(commands, sizeof(commands), stdin));
	} while(commands != "exit");
	exit(0);
}
