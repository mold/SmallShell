#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_INPUT_LEN 70
#define MAX_PARAM 35

int executeSync(char *command, char *args[]);
int executeAsync(char *command, char *args[]);

int checkChildrenStatus();
long getCurrentTimeMillis();

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
		
		char *newLine = strchr(g_input, '\n');
		*newLine = '\0';

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
		executeSync(g_params[0], g_params);

		/* Start async process */
	}
}

/**
 * Executes a synchronous (foreground) process.
*/
int executeSync(char *command, char *args[])
{
	/* Save start time */
	long startTime = getCurrentTimeMillis();

	/* fork */
	pid_t pid = fork();

	if(pid == 0)
	{
		/* child process */

		/* Execute command */
		execvp(command, args);		
		fprintf(stderr, "Execvp (%s) failed. errno: %d\n", command, errno);
		exit(1);
	}
	else if(pid == -1){
		/* Fork failed */
		fprintf(stderr, "Fork for %s failed. errno: %d\n", command, errno);
		exit(1);
	}
	else
	{
		/* Parent process */

		/* wait for child to terminate */
		int status;
		int res = waitpid(pid, &status, 0);
		
		if(res == -1)
		{	
			fprintf(stderr, "Wait for %d failed.\n", pid);
			exit(1);
		}

		/* print child status and execution duration */
		long durationTimeMillis = startTime - getCurrentTimeMillis();
		fprintf(stdout, "Stopped: %s (PID: %d) with status %d Runtime: %lo ms.\n", command, pid, status, durationTimeMillis);
	}

	return 0;
}

/**
 * Executes an asynchronous (background) process.
*/
int executeAsync(char *command, char *args[])
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

/**
* Returns the current time in milliseconds (since the Epoch).
*/
long getCurrentTimeMillis()
{
	struct timeval tv;
	int res = gettimeofday(&tv, NULL);

	/*	fprintf(stderr, "sec: %d usec: %d\n", tv.tv_sec, tv.tv_usec);	*/

	return (long)(tv.tv_sec*1000 + tv.tv_usec/1000);
}


