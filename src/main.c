/* Main File */
#include "simpleShell.h"

int main(void) {
    // Store current path
    char* currPath = getenv("PATH");
    // Set Working directory to HOME
    chwDir();
    readInput(currPath);
	return 0;
}