#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char * argv[]) {
	FILE * file1;
	FILE * file2;
	if (argc == 3) {
		file1 = fopen(argv[1], "r");
		if (file1 == NULL) return EXIT_FAILURE;
		file2 = fopen(argv[2], "r");
		if (file2 == NULL) return EXIT_FAILURE;
		
		
		
		size_t str_size = 0;
		char* str = NULL;
		getline(&str, &str_size, file1);
		int rows = atoi(str);
		strtok(str, "x");
		int cols = atoi(strtok(NULL, "x"));
		
		
		int *matrix1 = mmap(NULL, cols * rows * sizeof(int), PROT_READ|PROT_WRITE,
		MAP_SHARED|MAP_ANONYMOUS, 0, 0);
		if(matrix1 == MAP_FAILED){
			perror("mmap");
			exit(EXIT_FAILURE);
		}
		
		int *matrix2 = mmap(NULL, cols * rows * sizeof(int), PROT_READ|PROT_WRITE,
		MAP_SHARED|MAP_ANONYMOUS, 0, 0);
		if(matrix2 == MAP_FAILED){
			perror("mmap");
			exit(EXIT_FAILURE);
		}
		
		
		int i = 0, j = 0;
		while ( getline(&str, &str_size, file1) > 0 ) {
			char* token = strtok(str," ");
			while( token != NULL ) {
				matrix1[i * rows + j]=atoi(token);
				token=strtok(NULL," ");
				j++;
			}
			i++;
			j=0;
		}
		
		str_size = 0;
		str = NULL;
		getline(&str, &str_size, file2);
		i = 0;
		j = 0;
		while ( getline(&str, &str_size, file2) > 0 ) {
			char* token = strtok(str," ");
			while( token != NULL ) {
				matrix2[i * rows + j]=atoi(token);
				token=strtok(NULL," ");
				j++;
			}
			i++;
			j=0;
		}
		
		
		fclose(file1);
		fclose(file2);
		
		int *resultMatrix = mmap(NULL, cols * rows * sizeof(int), PROT_READ|PROT_WRITE,
		MAP_SHARED|MAP_ANONYMOUS, 0, 0);
		if(resultMatrix == MAP_FAILED){
			perror("mmap");
			exit(EXIT_FAILURE);
		}
		
		for(i = 0; i < cols * rows; i++) resultMatrix[i] = 0;
		
		for(i = 0; i < cols; i++) {
			pid_t pid;
			if ((pid = fork()) < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
			}
		if(pid == 0) {
			for(int j = 0; j < rows; j++)
				if (j % cols == i) {
					for(int k = 0; k < cols; k++) {
						resultMatrix[j * rows + k] = matrix1[j * rows + k] + matrix2[j * rows + k];
					}
			}
					
			exit(EXIT_SUCCESS);
			}
		}
		
		for(i = 0; i < cols; i++) {
			if ( waitpid(-1, NULL, 0) < 0) {
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
		}
		
		printf("%dx%d\n", rows, cols);
		for(i = 0; i < rows; i++){
			for(j = 0; j < cols; j++){
				printf("%d ", resultMatrix[i * rows + j]);
			}
			printf("\n");
		}
		
		if (munmap(resultMatrix, sizeof(resultMatrix)) < 0) {
			perror("munmap");
			exit(EXIT_FAILURE);
		}
		if (munmap(matrix1, sizeof(matrix1)) < 0) {
			perror("munmap");
			exit(EXIT_FAILURE);
		}
		if (munmap(matrix2, sizeof(matrix2)) < 0) {
			perror("munmap");
			exit(EXIT_FAILURE);
		}
		
		
	} else {
		printf("usage: %s file1 file2\n", argv[0]);
	}
	
	return EXIT_SUCCESS;
}
