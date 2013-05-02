debug:
	gcc -O0 -Wall -g -Wextra -ansi -o SmallShell smallshell.c
release:
	gcc -O2 -Wall -Wextra -ansi -o SmallShell smallshell.c
clean:
	rm SmallShell
