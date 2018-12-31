#include "prog2.h"

int main(int argc, char **argv){

	////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////// Set Up ////////////////////////////////////////////////
	
	long number_of_baby_eagles=10;				// Number of eagles 	
	long number_of_feedings=10;					// Number of time mother will fill the feeding pots
	long number_of_pots=10;						// Number of feeding pots available

	if (argc == 4){
		number_of_pots= atoi(argv[1])==0 ? 10 : atoi(argv[1]); 			 
		number_of_baby_eagles= atoi(argv[2])==0 ? 10 : atoi(argv[2]); 			
		number_of_feedings= atoi(argv[3])==0 ? 10 : atoi(argv[3]);	 		
	}

	sem_init(&mother_st, 0, 0);				// initialzie mother status to zero meaning mother is asleep
	sem_init(&full_pots, 0, 0);				// initialzie number of full feeding pots to zero
	sem_init(&non_empty_pots, 0, 0);		// initialzie number of non-empty feeding pots to zero
	sem_init(&can_eat, 0, 0);				// initialzie grant of eating from pots to zero
	sem_init(&waking_up, 0, 1);				// initialzie the grant of waking up mother eagle to one hence the first eagle that gets to this semaphore will be able to wake of the mother eagle

	printf("MAIN: There are %d baby eagles, %d feeding pots, and %d feedings.\n",number_of_baby_eagles ,number_of_pots ,number_of_feedings);
	printf("MAIN: Game starts!!!!!\n");

	retired=0;								// initialize the mother to not yet retired
	int rc;									// integer for error code return
	long i;									// integer used in loops
	int total_eagles = number_of_baby_eagles + 1;	// total number of eagles (mother and baby)
	pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t) *total_eagles);	// array to hold thread ids
	
	struct input_data *data = (struct input_data *)malloc(sizeof(struct input_data)); // struct to hold the data passed to mother eagle
	data->number_of_feedings = number_of_feedings;
	data->number_of_pots = number_of_pots;
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////// Start the Game ////////////////////////////////////////////

	printf("In main: creating Mother eagle thread %ld\n", 0);
	rc = pthread_create(threads + 0, NULL, mother_eagle, (void *) data);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}

	for (i = 1; i < total_eagles; i++) {
		printf("In main: creating baby eagle thread %ld\n", i);
		rc = pthread_create(threads + i, NULL, baby_eagle, (void *) i);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////  Clean up  /////////////////////////////////////////////

	// wait for all threads to complete 
	for (i = 0; i < total_eagles; i++) {
		pthread_join(threads[i], NULL);
	}

	// clean up allocated memory
	free(data);

	return 0;
}

