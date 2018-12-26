#include <stdio.h>
#include <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>



int main() {
	key_t key;
	int       shm_id;        /* shared memory ID      */   	
	int       *shm_ptr;

	key = ftok(".",'x');

	shm_id = shmget(key, 100 * sizeof(int), IPC_CREAT | 0666);
	if (shm_id < 0) {
	     printf("shmget error\n");
	     exit(1);
	}
	printf("Matrix multiplication with multiple processes:\n");
	printf("\n");
        printf("*** MAIN: shared memory key = %d\n",key);
        printf("*** MAIN: Main has received a shared memory of 100 integers\n");
 

	shm_ptr = (int *) shmat(shm_id, NULL, 0);  /* attach */
	if ((int) shm_ptr == -1) {
	     printf("*** shmat error (server) ***\n");
	     exit(1);
	}
	printf("*** MAIN: shared memory attached and is ready to use\n");

	printf("\n");
	printf("\n");
	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////
	FILE *fp;
	fp = fopen ("data.in","r");
	if (fp == NULL) {
	printf ("File not created okay");
	return 1;
	}

	int i,j;
	int d;	
	int l,m,m1,n;
	fscanf(fp,"%d", &l);
	fscanf(fp,"%d", &m);

	printf("Matrix A: %d rows and %d columns\n",l,m);

	for (i=0;i<l;i++){
		for (j=0;j<m;j++){
			fscanf(fp,"%d", &d);
			*(shm_ptr + (j + (i*m))) = d;
//			printf("   %2d", (j + (i*m)));
			printf("   %2d", d);
		}
			printf("\n");
	}	
	
	fscanf(fp,"%d", &m1);
	fscanf(fp,"%d", &n);

	printf("Matrix B: %d rows and %d columns\n",m1,n);
	for (i=0;i<m1;i++){
		for (j=0;j<n;j++){
			fscanf(fp,"%d", &d);
			*(shm_ptr + (i + (j*m1))+ (l*m1) ) = d;
//			printf("   %2d", (i + (j*m1))+ (l*m1));
			printf("   %2d", d);
		}
			printf("\n");
	}	


	///////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////


//	for (i=0;i<l;i++){
//		for (j=0;j<m;j++){
//			printf("   %2d", *(shm_ptr + (j + (i*m))));
//		}
//			printf("\n");
//	}
		printf("\n");
//	for (i=0;i<m;i++){
//		for (j=0;j<n;j++){
//			printf("   %2d", *(shm_ptr + (j  + (i*n))+ (l*m)));
//		}
//			printf("\n");
//	}	

	if (m1 == m){
		pid_t  pid;
		int    status;
	
		printf("*** MAIN: about to spawn processes\n");
		for(i=0;i<l;i++){
			for(j=0;j<n;j++){
				char *argv[7]; // reduce
				argv[0] = "./compute";
				argv[1] = (char *)malloc(sizeof(char) * 5);
				argv[2] = (char *)malloc(sizeof(char) * 5);
				argv[3] = (char *)malloc(sizeof(char) * 5);
				argv[4] = (char *)malloc(sizeof(char) * 5);
				argv[5] = (char *)malloc(sizeof(char) * 5);
				sprintf(argv[1], "%d", i);
				sprintf(argv[2], "%d",(j+l));
				sprintf(argv[3], "%d", l);
				sprintf(argv[4], "%d", m);
				sprintf(argv[5], "%d", n);
				argv[6] = NULL; // delete
				if ((pid = fork()) < 0) {     /* fork a child process           */
					printf("*** ERROR: forking child process failed\n");
					exit(1);
				}
			     	else if (pid == 0) {          /* for the child process:         */
					if (execvp(*argv, argv) < 0) {     /* execute the command  */
						printf("*** ERROR: exec failed\n");
						exit(1);
					}
				}
			}
		}

		for(i=0; i< n*l; i++){
			wait(NULL);
		}	
			printf("\n");



		printf("Matrix C: %d rows and %d columns\n",l,n);
		for (i=0;i<l;i++){
			for (j=0;j<n;j++){
				printf("   %2d", *(shm_ptr + (l*m*m*n) + (j + (i*n))));
			}
				printf("\n");
		}
	} else {
		printf("*** MAIN: number of columns of matrix A is not equal to number of rows of matrix B \n");			
		printf("*** MAIN: Input matrices can not be multiplied\n");
	}
	shmdt(shm_ptr);
	printf("Main has detached its shared memory...\n");
	shmctl(shm_id, IPC_RMID, NULL); 
	printf("Main has removed its shared memory...\n");
  
	fclose (fp);	
	printf("Main exits...\n");
	return 0;
}
