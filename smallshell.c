#include <stdio.h>

#define MAX_INPUT_LEN = 70

int executeSync(char *command, int argc, char *args[]);
int executeAsync(char *command, int argc, char *args[]);

int checkChildrenStatus();

int main(int argc, char *args[])
{
	char input[MAX_INPUT_LEN];

	/* Setup signal handler */

	while(true)
	{	
		/* Check for terminated child processes */

		/* Get next command */		
		
		/* Parse command */
		
		/* Start synchronous process */

		/* Start async process */
	}
}

/**
 * Executes a synchronous (foreground) process.
*/
int executeSync(char *command, int argc, char *args[])
{
	return 0;
}

/**
 * Executes an asynchronous (background) process.
*/
int executeAsync(char *command, int argc, char *args[])
{
	return 0;
}

/**
* Checks all children for any terminated processes and prints
* information about them.
*/
int checkChildrenStatus()
{
	return 0;
}
