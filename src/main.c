/* Main File */
#include "simpleShell.h"

int main(void) {
    // Store current path
    char* currPath = getPath();
    readInput(currPath);
	return 0;
}