debug:
	gcc -O0 -Wall -g -Wextra -o SmallShell smallshell.c
release:
	gcc -O2 -Wall -Wextra -o SmallShell smallshell.c
clean:
	rm SmallShell
