/*
 * NAME:
 *	SmallShell	-	A small shell/command line interface for UNIX.
 *
 * SYNTAX:
 *	SmallShell
 *
 * DESCRIPTION:
 *
 *
 * AUTHOR:
 *	Daniel Molin <dmol@kth.se>
 *	Christian Magnerfelt <christian.magnerfelt@gmail.com>
 */
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

/* Checks and prints status of background child processes that have exited */
void checkChildrenStatus();

/* Parses parameters from input array */
bool parseParams();

/* Executes 'command' synchronously but in a separate process using 'args' as arguments*/
void executeSync(char *command, char *args[]);

/* Executes 'command' asynchronously in a separate process using 'args' as arguments*/
void executeAsync(char *command, char *args[]);

/* Gets the current timestamp in milliseconds */
long getCurrentTimeMillis();

/* global variables */
char * g_params [MAX_PARAM];	/* Array of pointers containing parameters parsed from command line */
char g_input[MAX_INPUT_LEN];	/* Temporary array for storing input from command line */
int g_numParams;				/* Number of parameters parsed */
int g_numProcesses;				/* Number of active background processes */

/* Program entry point */
int main()
{
	/* Setup signal handler */

	g_numProcesses = 0;		/* Set number of processes running in background to zero */
	bool isRunning = true;
	while(isRunning)
	{
		/* Check for terminated child processes */
		checkChildrenStatus();

		printf("$");
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

		/* Do we want to change directory? */
		if(!strcmp(g_params[0], "cd"))
		{
			if(g_numParams < 2)
			{
				/* Did supply directory path, change to home directory */
				chdir(getenv("HOME"));
			}
			else
			{
				int ret = chdir(g_params[1]);
				if(ret != 0)
				{
					chdir(getenv("HOME"));
				}
			}
			char buffer [255];
			if(getcwd(buffer, 255) != NULL)
			{
				printf("Current directory: %s\n", buffer);
			}
			continue;
		}
		/* Check if there is a trailing ampersand in the params, ampersand == run process asynchronously */
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
		/* Start asynchronus process */
		else
		{
			executeAsync(g_params[0], g_params);
		}
	}
	return 0;
}
/**
* Checks all children for any terminated processes and prints
* information about them.
*/
void checkChildrenStatus()
{
	/* Check if any child have terminated */
	int i;
	int exitCount = 0;
	for(i = 0; i < g_numProcesses; i++)
	{
		int status;
		pid_t id = waitpid(-1, &status, WNOHANG );

		if(id == -1)
		{
			fprintf(stderr, "Wait failed.\n");
			exit(1);
		}
		else if(id == 0)
		{
			/* Nothing have happend yet continue */
			continue;
		}
		if(WIFEXITED(status))
		{
			exitCount++; /* increase the exit count */
			fprintf(stdout, "Background process (PID: %d) terminated with status %d\n", id, status);
		}
	}
	/* Subtract the processes that have exited */
	g_numProcesses -= exitCount;
}
/*
 *	Parses parameters from input array and stores them in a array of pointers.
 *
 *	Returns true on success and false otherwise (for faulty input e.g. no command name)
 */
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
/*
 * Executes a synchronous (foreground) process.
 * 'command' The command to execute
 * 'args' The supplied arguments to 'command'
 */
void executeSync(char *command, char *args[])
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
		printf("Spawned foreground process pid: %d\n", pid);
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
		fprintf(stdout, "Foreground process (PID: %d) terminated with status %d\nWallclock time: %lo ms.\n",
			pid, status, durationTimeMillis);
	}
}

/*
 * Executes an asynchronous (background) process.
 * 'command' The command to execute
 * 'args' The supplied arguments to 'command'
 */
void executeAsync(char *command, char *args[])
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
		printf("Spawned background process pid: %d\n", pid);
		g_numProcesses++;
	}
}
/**
* Returns the current time in milliseconds (since the Epoch).
*/
long getCurrentTimeMillis()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return (long)(tv.tv_sec*1000 + tv.tv_usec/1000);
}


