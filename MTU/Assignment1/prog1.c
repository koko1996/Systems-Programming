#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

struct compare {
   long   first;
   long   second;
};


int *values;
int *input;

void *init_array(void *id_ptr)
{
	long id = (long) id_ptr;
//	printf("Thread number %ld %d\n", pthread_self(),id);
	values[id]=1;	
	return NULL;
}

void *compare_array(void *str_ptr)
{
	struct compare * str = (struct compare *) str_ptr;

	if (input[str->first] >  input[str->second]){	
		values[str->second]=0;	
	printf("Thread T(%d,%d) compares x[%d] = %d and x[%d] = %d and writes %d into w[%d]\n", str->first, str->second, str->first, input[str->first], str->second, input[str->second], 0 , str->second);
	} else if (input[str->first] < input[str->second]){
		values[str->first]=0;	
	printf("Thread T(%d,%d) compares x[%d] = %d and x[%d] = %d and writes %d into w[%d]\n", str->first, str->second, str->first, input[str->first], str->second, input[str->second], 0 , str->first);
	}
	return NULL;
}

void *max_array(void *id_ptr)
{
	long id = (long) id_ptr;
//	printf("Thread number %ld %d values[%d]=%d input[%d]= %d\n", pthread_self(),id, id,values[id],id,input[id]);
	if (values[id]==1) {
		printf("Maximum                = %d\n", input[id]);
		printf("Location               = %d\n", id);
	}
	
	return NULL;
}



int main(int argc, char **argv){
	int NUM_THREADS;	
	int NUM_ARGS;
	clock_t start, end;
	start = clock();

	NUM_ARGS = argc-1;	
	input= (int *)malloc(sizeof(int) * NUM_ARGS);
	values= (int *)malloc(sizeof(int) * NUM_ARGS);
	long i;		
	printf("Number of input values = %d\n",NUM_ARGS);
	printf("Input values         x =");	
	for(i=0;i<NUM_ARGS;i++){
		input[i]=atoi(argv[i+1]);
		printf(" %d",input[i]);
	}
	printf("\n");


	////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	NUM_THREADS = argc-1;	
	pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t) *NUM_THREADS);

//	printf("Before Thread Step 1\n");
	for (i = 0; i < NUM_THREADS; i++) {
//		printf("In main: creating thread %ld\n", i);
		if (pthread_create(threads + i, NULL, init_array, (void *) i)) {
			printf("ERROR; during pthread_create()\n");
			exit(-1);
		}
	}
	/* wait for all threads to complete */
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}
//	printf("After Thread Step 1\n");

	
	printf("After initialization w =");
	for(i=0;i<NUM_ARGS;i++){
		printf(" %d",values[i]);	
	}
	printf("\n");


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	NUM_THREADS=((NUM_THREADS * (NUM_THREADS -1))/2);
	threads = (pthread_t *) realloc(threads, sizeof(pthread_t) * NUM_THREADS );
	
	struct compare *str_array = (struct compare *) malloc(sizeof(struct compare) * NUM_THREADS );
//	printf("Before Thread Step 2\n");
	int j;
	int thread_i=0;
	for (i = 0; i < argc-1; i++) {
		for (j = i+1; j < argc-1; j++) {
//			printf("In main: creating thread %d for %d at threads[%d]\n", i, j ,thread_i);
			(str_array+thread_i)->first = i;
			(*(str_array+thread_i)).second = j;
			if (pthread_create(threads + thread_i, NULL, compare_array, (void *) (str_array+thread_i))) {
				printf("ERROR; during pthread_create()\n");
				exit(-1);
			} 
			thread_i++;
		}
	}
	/* wait for all threads to complete */

//	printf("Waiting For Step 2 Threads to Complete (%d threads %d thread_i)\n",NUM_THREADS,thread_i);
	for (i = 0; i < NUM_THREADS; i++) {
//		printf("Waiting For %dth thread to Complete\n",i);
		pthread_join(threads[i], NULL);
	}
//	printf("After Thread Step 2\n");

	free(str_array);

	printf("After comparision w =");
	for(i=0;i<NUM_ARGS;i++){
		printf(" %d",values[i]);	
	}
	printf("\n");

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	NUM_THREADS = NUM_ARGS;
	threads = (pthread_t *) realloc(threads, sizeof(pthread_t) * NUM_THREADS);

//	printf("Before Thread Step 3\n");
	for (i = 0; i < NUM_THREADS; i++) {
//		printf("In main: creating thread %ld\n", i);
		if (pthread_create(threads + i, NULL, max_array, (void *) i)) {
			printf("ERROR; during pthread_create()\n");
			exit(-1);
		}
	}
	/* wait for all threads to complete */
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

//	printf("After Thread Step 3\n");



	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	free(input);
	free(values);
	free(threads);
	end = clock();
//	printf("took %f seconds to execute \n", end - start);
	return 0;
}

