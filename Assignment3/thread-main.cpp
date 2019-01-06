#include "thread.h"

int main(int argc, char **argv){

	////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////// Set Up ////////////////////////////////////////////////
	long total_threads;
	const long number_of_boats=1;			
	long number_of_cannibals=2;				
	long number_of_missionaries=2;			
	long number_of_safe_boat_loads=5;		

	if (argc == 4){
		number_of_cannibals= atoi(argv[1])==0 ? 8 : atoi(argv[1]); 			 
		number_of_missionaries= atoi(argv[2])==0 ? 8 : atoi(argv[2]); 			
		number_of_safe_boat_loads= atoi(argv[3])==0 ? 5 : atoi(argv[3]);	 		
    }

    total_threads = number_of_boats + number_of_cannibals + number_of_missionaries;
	
    pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t) * total_threads); // array to hold thread ids
	int rc; // integer for error code return
	long i; // integer used in loops

	////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// Create threads ////////////////////////////////////////////

	printf("In main: creating Boat thread %ld\n", 0);
	rc = pthread_create(threads + 0, NULL, boat, (void *) number_of_safe_boat_loads);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}

	for (i = 1; i <= number_of_cannibals; i++) {  // number_of_boats = 1 by default
		printf("In main: creating Cannibal thread %ld\n", i );
		rc = pthread_create(threads + i , NULL, cannibal, (void *) i);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
    }

	for (i = 1; i <= number_of_missionaries; i++) {
		printf("In main: creating Missionaries thread %ld\n", number_of_cannibals + i );
		rc = pthread_create(threads + number_of_cannibals + i , NULL, missionary, (void *) i);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
    }



	///////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////  Clean up  /////////////////////////////////////////////

	// wait for all threads to complete 
	for (i = 0; i < total_threads; i++) {
		pthread_join(threads[i], NULL);
	}

	// clean up allocated memory
	free(threads);
    
    return 0;
}