#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

sem_t mother_st; // mother eagles
sem_t waking_up; // mutex for waking up the mother
sem_t filled_pots; // number of pots that are full (no baby eagle touched it yet)
sem_t busy_pots; // number of pots that is occupied by the baby eagles (they are still eating)
sem_t can_eat; // baby eagles wait until mother fills up all the pots

long babyWaker;

int NUM_THREADS;
int n;
int retired;


void mother_goto_sleep(){
	printf("Mother eagle takes a nap.\n");
	sem_wait (&mother_st);
}

void food_ready(int t){
	printf("Mother eagle says \"Feeding (%d)\".\n",t);
}


void finish_eating(long id){
	int value3;
	sem_wait(&can_eat);	
	sem_getvalue(&busy_pots, &value3);
	if (value3 <= 0){
		printf("%*cBaby eagle %d exited.\n",id,' ',id);
		sem_post(&can_eat);
		pthread_exit(0);	
	}
	printf("%*cBaby eagle %d is starts eating.\n",id,' ',id);
	sem_wait (&busy_pots);
	sem_post(&can_eat);
	printf("%*cBaby eagle %d finishes eating.\n",id,' ',id);
}

void ready_to_eat(long id){
	int value1;
	int value2;	
	int value3;
	printf("%*cBaby eagle %d is ready to eat.\n",id,' ',id);
	sem_wait(&waking_up);
	sem_getvalue(&busy_pots, &value1);
	sem_getvalue(&mother_st, &value2);
	sem_getvalue(&waking_up, &value3);
//	printf("before value1 == %d && value2 ==%d && value3=%d && retired ==%d.\n",value1, value2,value3, retired);
	if (value1 == 0 && value2 ==0 && retired ==0){
		printf("Baby eagle %d sees all feeding pots are empty and wakes up the mother.\n",id);
		babyWaker = id;
		sem_post(&mother_st);
		sem_getvalue(&mother_st, &value2);
//		printf("after value1 == %d && value2 ==%d && retired ==%d.\n",value1, value2, retired);
	}
	sem_post(&waking_up);
	sem_wait(&filled_pots);
}

void *baby_eagle(void * baby_id) {
	long id = (long) baby_id;
	printf("%*cBaby eagle %d started.\n",id,' ',id);
	while (1) {              
		sleep(2);            // play for a while     
		ready_to_eat(id);  // get hungry          
		sleep(2);            // eat for a while     
		finish_eating(id); // finish eating       
	}
}

void *mother_eagle(void *t_times) {
	printf("Mother eagle started.\n");
	long t;
	t = (long) t_times;
	int i=1;
	int temp;
	while (i <= t) {            
		mother_goto_sleep();    // take a nap
         	printf("Mother eagle woken up by baby eagle %d.\n",babyWaker);
		sem_wait(&can_eat);
		temp=0;
		while(temp<n){
			sem_post (&filled_pots);
			sem_post (&busy_pots);
			temp++;
		}
		sleep(1);      
		food_ready(i);    // make food available
		sem_post(&can_eat);
	//	sleep(5);            // do something else  
		i++;
	}
	
	retired =1;
	printf("Mother eagle retired Game Over\n");
	for (i = 1; i < NUM_THREADS; i++) {
		sem_post (&filled_pots);
	}
	
	
}

int main(int argc, char **argv){
	int m=4;
	n=2;
	long t=2;	
	if (argc == 4){
		m=atoi(argv[1]);
		n=atoi(argv[2]);
		t=atoi(argv[3]);
	}
	sem_init(&mother_st, 0, 0);
	sem_init(&filled_pots, 0, 0);
	sem_init(&busy_pots, 0, 0);
	sem_init(&can_eat, 0, 1);
	sem_init(&waking_up, 0, 1);	
	retired=0;

	printf("MAIN: There are %d baby eagles, %d feeding pots, and %d feedings.\n",m ,n ,t);
	printf("MAIN: Game starts!!!!!\n");

	////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	
	int rc;
	NUM_THREADS= m + 1;
	pthread_t * threads = (pthread_t *)malloc(sizeof(pthread_t) *NUM_THREADS);
	printf("In main: creating Mother eagle thread %ld\n", 0);
	rc = pthread_create(threads + 0, NULL, mother_eagle, (void *) t);
	if (rc) {
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}

	long i;
	for (i = 1; i < NUM_THREADS; i++) {
		printf("In main: creating baby eagle thread %ld\n", i);
		rc = pthread_create(threads + i, NULL, baby_eagle, (void *) i);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}

	int value1;
	sem_getvalue(&filled_pots, &value1);
	while (value1>0 || retired==0) {     
	
		sem_getvalue(&filled_pots, &value1);
	}
	////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////

	/* wait for all threads to complete */
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
		printf("Thread %d is done\n",i);
	}
	printf("After Thread\n");

	return 0;
}

