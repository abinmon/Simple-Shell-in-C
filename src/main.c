/* Main File */
#include <stdio.h>
#include <stdlib.h>
#include "simpleShell.h"
#define ARG_MAX 32000

int main(void) {
	char commands[ARG_MAX];
	init();
  	readInput(commands);
	exit(0);
}
