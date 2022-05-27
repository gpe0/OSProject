all: cypher
 
cypher: ex3.o wordsProcess.o
	gcc -o cypher ex3.o wordsProcess.o
 
ex3.o: ex3.c wordsProcess.h
	gcc -o ex3.o ex3.c -c -Wall -Werror -std=c99 -D_DEFAULT_SOURCE

wordsProcess.o: wordsProcess.c wordsProcess.h
	gcc -o wordsProcess.o wordsProcess.c -c -Wall -Werror -std=c99 -D_DEFAULT_SOURCE

clean:
	rm -rf *.o *~ cypher
