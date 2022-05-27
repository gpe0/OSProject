all: addmx
 
addmx: ex2.o
	gcc -o addmx ex2.o
 
ex2.o: ex2.c
	gcc -o ex2.o ex2.c -c -Wall -Werror -std=c99 -D_DEFAULT_SOURCE
 
clean:
	rm -rf *.o *~ addmx
