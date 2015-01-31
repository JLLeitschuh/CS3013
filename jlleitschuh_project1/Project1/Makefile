all: runCommand shell shell2

shell2: shell2.o
	gcc shell2.o -o shell2

shell: shell.o
	gcc shell.o -o shell

runCommand: runCommand.o
	gcc runCommand.o -o runCommand

shell2.o: shell2.c
	gcc shell2.c -c

shell.o: shell.c
	gcc shell.c -c

runCommand.o: runCommand.c
	gcc runCommand.c -c

clean:
	rm -f *.o runCommand shell shell2
