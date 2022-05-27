all: phrases
 
phrases: ex1.o trimspaces.o
	gcc -o phrases ex1.o trimspaces.o
 
ex1.o: ex1.c trimspaces.h
	gcc -o ex1.o ex1.c -c -Wall -Werror -std=c99 -D_DEFAULT_SOURCE
 
trimspaces.o: trimspaces.c trimspaces.h
	gcc -o trimspaces.o trimspaces.c -c -Wall -pedantic -std=c99 -D_DEFAULT_SOURCE
 
clean:
	rm -rf *.o *~ phrases
