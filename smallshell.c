#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAX_INPUT_LEN 70
#define MAX_PARAM 35

int executeSync(char *command, int argc, char *args[]);
int executeAsync(char *command, int argc, char *args[]);

int checkChildrenStatus();

char * g_params [MAX_PARAM];
char g_input[MAX_INPUT_LEN];

int main(int argc, char *args[])
{
	/* Setup signal handler */

	bool isRunning = true;
	while(isRunning)
	{	
		/* Check for terminated child processes */

		/* Get next command */
		if(fgets(g_input, MAX_INPUT_LEN, stdin) == NULL)
			continue;
		//fprintf(stderr, "Input: %s", input);
		
		/* Parse command */
		{
			char * param = strtok(g_input, " ");
			int count = 0;
			while(param != NULL)
			{
				g_params[count] = param;
				//printf("%s\n", g_params[count]);
				count++;		
				param = strtok(NULL, " ");
			}
			g_params[count] = (char *) NULL;
		}
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
