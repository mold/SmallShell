#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_INPUT_LEN 70
#define MAX_PARAM 35

int executeSync(char *command, char *args[]);
int executeAsync(char *command, char *args[]);

int checkChildrenStatus();
long getCurrentTimeMillis();

char * g_params [MAX_PARAM];
char g_input[MAX_INPUT_LEN];
int g_numParams;
int g_numProcesses;

bool parseParams();

int main()
{
	/* Setup signal handler */

	g_numProcesses = 0;		/* Set number of processes running in background to zero */
	bool isRunning = true;
	while(isRunning)
	{	
		/* Check for terminated child processes */
		checkChildrenStatus();

		/* Get next command */
		if(fgets(g_input, MAX_INPUT_LEN, stdin) == NULL)
			continue;

		/* Replace end-line character with null-terminator */
		char *newLine = strchr(g_input, '\n');
		*newLine = '\0';

		/* Parse params from input */
		if(!parseParams())
			continue;

		/* Do we want to exit the shell? */
		if(!strcmp(g_params[0], "exit"))
		{
			isRunning = false;
			continue;
		}

		/* Check if there is a trailing tilda in the params, tilda == run process asynchronously */
		bool isAsync = false;
		if(g_numParams > 1)
		{
			if(!strcmp(g_params[g_numParams - 1], "&"))
			{
				g_params[g_numParams - 1] = (char *) NULL;
				isAsync = true;
			}
		}
		/* Start synchronous process */
		if(!isAsync)
		{
			executeSync(g_params[0], g_params);
		}
		/* Start asynchrouns process */
		else
		{
			executeAsync(g_params[0], g_params);
		}
	}
	return 0;
}
bool parseParams()
{
	char * param = strtok(g_input, " ");
	/* Check if have characters in input */
	if(param == NULL)
		return false;	/* No characters, parse fail */

	/* Tokenize input and store ptr to string in global params */
	int count = 0;
	while(param != NULL)
	{
		g_params[count] = param;
		count++;		
		param = strtok(NULL, " ");
	}

	g_params[count] = (char *) NULL;
	g_numParams = count;
	return true;
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
		long durationTimeMillis = getCurrentTimeMillis() - startTime;
		fprintf(stdout, "Stopped: %s (PID: %d) with status %d Runtime: %lo ms.\n", command, pid, status, durationTimeMillis);
	}

	return 0;
}

/**
 * Executes an asynchronous (background) process.
*/
int executeAsync(char *command, char *args[])
{
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
		g_numProcesses++;
	}

	return 0;
}

/**
* Checks all children for any terminated processes and prints
* information about them.
*/
int checkChildrenStatus()
{
	/* Check if any child have terminated */
	int i;
	for(i = 0; i < g_numProcesses; i++)
	{
		int status;
		pid_t id = waitpid(-1, &status, WNOHANG);

		if(id == -1)
		{	
			continue;
		}
		if(WIFEXITED(status))
		{
			g_numProcesses--;
			fprintf(stdout, "Background process (PID: %d) terminated with status %d\n", id, status);
		}
	}
	return 0;
}

/**
* Returns the current time in milliseconds (since the Epoch).
*/
long getCurrentTimeMillis()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	/*	fprintf(stderr, "sec: %d usec: %d\n", tv.tv_sec, tv.tv_usec);	*/

	return (long)(tv.tv_sec*1000 + tv.tv_usec/1000);
}


