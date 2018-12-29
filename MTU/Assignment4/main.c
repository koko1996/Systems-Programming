#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <sys/wait.h>

#define INPUT_FILE_NAME "data2.in"
#define MATRIX_MULT "./compute"


// @brief gets a file pointer and returns the size of the file 
// @param fp is a pointer to a file
// @return long that represents the number of elements in the file
long fsize(FILE *fp){
    long prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long sz=ftell(fp);
    fseek(fp,prev,SEEK_SET); //go back to where we were
    return sz;
}

int main() {
	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////  Set Up  //////////////////////////////////////////
	// local variables
	FILE *fp;
	int i,j,k;									// integer for loops
	int d;										// integer for values read
	int l,m,m1,n;								// dimensions of the matrices
   	long size;									// size neccessary for shared memory

   	fp = fopen (INPUT_FILE_NAME,"r");			// open the file for reading
	if (fp == NULL) {
		printf ("Input file not created");
		return 1;
	}
	// get current file size to allocate memory with necessary size and duplicate that in order to save the results as well
	size = 2 * fsize(fp); 							
    printf("*** SIZE = %d\n",size);

	key_t key;					// key for the shared memory
	int       shm_id;        	// shared memory ID      	
	int       *shm_ptr;			// pointer to the shared memory 

	key = ftok(".",'x');		// generate the key (the other process must use the pass the same parameters to ftok )
	// create the shared memory with read and write permissions and with size matching the input file size
	shm_id = shmget(key,  size * sizeof(int), IPC_CREAT | 0666); 
	if (shm_id < 0) {
	     printf("shmget error\n");
	     exit(1);
	}

	printf("Matrix multiplication with multiple processes:\n");
	printf("\n");
        printf("*** MAIN: shared memory key = %d\n",key);
        printf("*** MAIN: Main has received a shared memory of %ld integers\n",size);
 
 	// attach to the created shared memory	 
	shm_ptr = (int *) shmat(shm_id, NULL, 0); 
	if ( *shm_ptr == -1) {
	     printf("*** shmat error (server) ***\n");
	     exit(1);
	}
	printf("*** MAIN: shared memory attached and is ready to use\n\n\n");



	///////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////// Read the input ///////////////////////////////////////

	fscanf(fp,"%d", &l);						//	read the number of rows of the first matrix
	fscanf(fp,"%d", &m);						//	read the number of columns of the first matrix

	printf("Matrix A: %d rows and %d columns\n",l,m);
	// read the elements from the file to shared memory
	for (i=0;i<l;i++){
		for (j=0;j<m;j++){
			fscanf(fp,"%d", &d);
			*(shm_ptr + (j + (i*m))) = d;
			printf("   %2d", d);
		}
			printf("\n");
	}	
	
	fscanf(fp,"%d", &m1);						//	read the number of rows of the second matrix
	fscanf(fp,"%d", &n);						//	read the number of columns of the second matrix

	printf("Matrix B: %d rows and %d columns\n",m1,n);
	// read the elements from file to shared memory while transpoing it (save the transpose of the matrix)
	for (i=0;i<m1;i++){
		for (j=0;j<n;j++){
			fscanf(fp,"%d", &d);
			*(shm_ptr + (i + (j*m1))+ (l*m1) ) = d;
			printf("   %2d", d);
		}
			printf("\n");
	}	
	printf("\n");
	
	// close file descriptor 
	fclose (fp);	
	///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////// Multiplication /////////////////////////////////////

	// proceed with multiplication if the number of columns of the first matrix matches the number of rows of the seconds matrix 
	if (m1 == m){
		pid_t  pid;
		int    status;
		
		// arguments to be passed to the child process
		char ** arguments  = (char **) malloc(sizeof(char*) * 7);
		arguments[0] = MATRIX_MULT;				// name of the child process to be called 
		arguments[1] = (char *)malloc(sizeof(char) * 5);
		arguments[2] = (char *)malloc(sizeof(char) * 5);
		arguments[3] = (char *)malloc(sizeof(char) * 5);
		arguments[4] = (char *)malloc(sizeof(char) * 5);
		arguments[5] = (char *)malloc(sizeof(char) * 5);
		sprintf(arguments[3], "%d", l);			// number of rows of the first matrix
		sprintf(arguments[4], "%d", m);			// number of rows of the second matrix (columns of first matrix)
		sprintf(arguments[5], "%d", n);			// number of columns of the second matrix
		arguments[6] = NULL; 

		printf("*** MAIN: about to spawn processes\n");
		for(i=0;i<l;i++){
			for(j=0;j<n;j++){
				sprintf(arguments[1], "%d", i);			//	row that the child is responsible to multiply
				sprintf(arguments[2], "%d",(j+l));		//	column that the child is responsible to multiply

				if ((pid = fork()) < 0) {    			 // fork child process
					printf("*** ERROR: forking child process failed\n");
					exit(1);
				}
				else if (pid == 0) {          			// in the child process:        
					if (execvp(*arguments, arguments) < 0) {     	// execute the command  
						printf("*** ERROR: exec failed\n");
						exit(1);
					}
				} 
			}
		}

		// clean up the allocated memory
		for (k=1;k<6;k++){
			free(arguments[k]);
		}
		free(arguments);

		// wait for the child processes to finish
		for(i=0; i< n*l; i++){
			wait(NULL);
		}	
		printf("\n");


		////////////////////////////////// Result ///////////////////////////////////////////
		// Print the result of the multiplication by reading from the shared memory
		printf("Matrix C: %d rows and %d columns\n",l,n);
		for (i=0;i<l;i++){
			for (j=0;j<n;j++){
				printf("   %6d", *(shm_ptr + (l*m*m*n) + (j + (i*n))));
			}
				printf("\n");
		}

	} else {
		printf("*** MAIN: number of columns of matrix A is not equal to number of rows of matrix B \n");			
		printf("*** MAIN: Input matrices can not be multiplied\n");
	}


	///////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////// Clean Up ///////////////////////////////////////////

	// detach shared memory
	shmdt(shm_ptr);
	printf("Main has detached its shared memory...\n");
	// remove the share memory
	shmctl(shm_id, IPC_RMID, NULL); 
	printf("Main has removed its shared memory...\n");
	printf("Main exits...\n");

	return 0;
}
