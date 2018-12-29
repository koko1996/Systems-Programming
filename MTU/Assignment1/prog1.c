#include "prog1.h"

int main(int argc, char **argv){
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////Set UP////////////////////////////////////////////////

	int NUM_THREADS = argc -1;				// Number of threads needed
	int NUM_ARGS = argc-1; 					// Number of integers to be compared 
	long i;									// integer value for loops
	long j;									// integer value for loops
	long thread_i;							// integer value needed in Comparision stage

	input= (int *)malloc(sizeof(int) * NUM_ARGS);								// allocate the global array that holds the input values
	values= (int *)malloc(sizeof(int) * NUM_ARGS);								// allocate the global array that holds the values of comparision
	pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t) *NUM_THREADS);	// allocate the local array that holds the thread ids

	struct compare *str_array;											// array of compare structs that defines which elements to compare
		

	////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////// First Print stage////////////////////////////////////////////

	printf("Number of input values = %d\n",NUM_ARGS);
	printf("Input values         x =");	
	for(i=0;i<NUM_ARGS;i++){
		input[i]=atoi(argv[i+1]);
		printf(" %d",input[i]);
	}
	printf("\n");


	////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////Initialize the array to defaul////////////////////////////////

	// create a new thread on each iteration, where each thread assigns 1 to it's position in the values array
	for (i = 0; i < NUM_THREADS; i++) {
		if (pthread_create(threads + i, NULL, init_array, (void *) i)) {
			printf("ERROR; during pthread_create()\n");
			exit(-1);
		}
	}

	//  wait for all threads to complete 
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
	
	// print the array after initialization is done
	printf("After initialization w =");
	for(i=0;i<NUM_ARGS;i++){
		printf(" %d",values[i]);	
	}
	printf("\n");


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////// Comparision stage //////////////////////////////////////////////

	NUM_THREADS=((NUM_THREADS * (NUM_THREADS -1))/2); 	// The number of Threads needed in this stage
	threads = (pthread_t *) realloc(threads, sizeof(pthread_t) * NUM_THREADS );	// resize the array since the number of threads increased
	
	// allocate the array
	str_array = (struct compare *) malloc(sizeof(struct compare) * NUM_THREADS );
	thread_i=0;
	
	// create a new thread on each iteration that calls compare_array function, where each thread compares two elements of the input array
	// and sets 0 if the value compared is less than the other value (two values are compared)
	// no synchronization needed since all the setter threads are trying to set to 0 
	for (i = 0; i < argc-1; i++) {
		for (j = i+1; j < argc-1; j++) {
			(str_array+thread_i)->first = i;			// inialize the fist element to be compared 
			(str_array+thread_i)->second = j;			// inialize the second element to be compared 
			if (pthread_create(threads + thread_i, NULL, compare_array, (void *) (str_array+thread_i))) {
				printf("ERROR; during pthread_create()\n");
				exit(-1);
			} 
			thread_i++;
		}
	}
	

	//	Wait for comparision threads to complete 
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	// clean up allocated memory since it is not needed anymore
	free(str_array);


	// print the array after comparision is done
	printf("After comparision w =");
	for(i=0;i<NUM_ARGS;i++){
		printf(" %d",values[i]);	
	}
	printf("\n");

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////// Find the Maximum  ////////////////////////////////////////////

	NUM_THREADS = NUM_ARGS;									// The number of threads needed in this stage
	threads = (pthread_t *) realloc(threads, sizeof(pthread_t) * NUM_THREADS);  // resize the array since the number of threads increased

	// create a new thread on each iteration that calls max_array function, which in turn prints out the maximum value
	for (i = 0; i < NUM_THREADS; i++) {
		if (pthread_create(threads + i, NULL, max_array, (void *) i)) {
			printf("ERROR; during pthread_create()\n");
			exit(-1);
		}
	}


	// wait for all threads to complete 
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////Clean Up///////////////////////////////////////////////////
	free(input);
	free(values);
	free(threads);

	return 0;
}

