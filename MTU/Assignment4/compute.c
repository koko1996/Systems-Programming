#include <stdio.h>
#include <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>



int main( int argc, char *argv[] )  {

	key_t key;
	int       shm_id;        /* shared memory ID      */   	
	int       *shm_ptr;

	int m = atoi(argv[1]);
	int n = atoi(argv[2]);

	int L = atoi(argv[3]);
	int M = atoi(argv[4]);
	int N = atoi(argv[5]);

	printf("   ### PROC(%d): entering with row %d and column %d\n",getpid(),m,n-L);
	key = ftok(".", 'x');
	shm_id = shmget(key, 100 * sizeof(int), 0666);
	
	if (shm_id < 0) {
		printf("*** shmget error (client) ***\n");
		exit(1);
	}
//	printf("   Client has received a shared...\n");

	shm_ptr =  (int *) shmat(shm_id, NULL, 0);  /* attach */
	if ((int) shm_ptr == -1) {
		printf("*** shmat error (client) ***\n");
		exit(1);
	}
//	printf("   Client has attached the shared memory...\n");

	int ans =0 ; 	
	int i;
	for (i=0;i<M;i++){
		ans += ( (*(shm_ptr + i + (M * m)))  *  (*(shm_ptr + i +  (M * n)))  );
	}	

	printf("   ### PROC(%d): C[%d][%d]=%d\n",getpid(),m,n-L,ans);
	*(shm_ptr + (L*M*M*N) + ( (m * N) + (n-L) )  ) = ans;
	shmdt((void *) shm_ptr);
//	printf("   Client has detached its shared memory...\n");
//	printf("   Client exits...\n");
	return 0;
}
