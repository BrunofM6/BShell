all: bshell.c
				cc -Wall -Werror -o bshell bshell.c

clean:
		rm bshell