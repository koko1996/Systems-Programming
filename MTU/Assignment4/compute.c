#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>


int main( int argc, char *argv[] )  {
	int ans =0 ; 		// result of the multiplication
	int i;				// integer for loop

	key_t key;					// key for the shared memory
	int       shm_id;        	// shared memory ID      	
	int       *shm_ptr;			// pointer to the shared memory 
	
	int row = atoi(argv[1]);			// row to multiply
	int column = atoi(argv[2]);			// column to multiply
	int L = atoi(argv[3]);				// number of rows of the first matrix
	int M = atoi(argv[4]);				// number of rows of the second matrix (columns of first matrix)
	int N = atoi(argv[5]);				// number of columns of the second matrix

	printf("   ### PROC(%d): entering with row %d and column %d\n",getpid(),row,column-L);
	
	key = ftok(".",'x');		// generate the key with the same parameters as the parent
	//	get the id of the shared with the specified key (shared memory allocated by the parent process with the same key)
	shm_id = shmget(key, 128 * sizeof(int), 0666);	
	if (shm_id < 0) {
		printf("*** shmget error (client) ***\n");
		exit(1);
	}

	//	attach to shared memory
	shm_ptr =  (int *) shmat(shm_id, NULL, 0);  /* attach */
	if ( *shm_ptr == -1) {
		printf("*** shmat error (client) ***\n");
		exit(1);
	}

	// calculate the by multiplying the mth row and nth column from shared memory
	for (i=0;i<M;i++){
		ans += ( (*(shm_ptr + i + (M * row)))  *  (*(shm_ptr + i +  (M * column)))  );
	}	
	printf("   ### PROC(%d): C[%d][%d]=%d\n",getpid(),row,column-L,ans);

	// save the result to shared memory
	*(shm_ptr + (L*M*M*N) + ( (row * N) + (column-L) )  ) = ans;
	
	//	detach from shared memory
	shmdt((void *) shm_ptr);

	return 0;
}
