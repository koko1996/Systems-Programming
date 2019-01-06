#include "prog2.h"


// @brief ready_to_eat indicates that baby eagle is ready to eat. The baby eagle starts by checking if there is any available pot to eat from and if not then wakes up the mother and waits until mother allows this eagle to continue eating
// @param id long indicating the id of the caller thread
void ready_to_eat(long id){
	int number_of_full_pots;
	int number_of_non_empty_pots;
	
	printf("%*cBaby eagle %d is ready to eat.\n",id,' ',id);
	
	sem_wait(&waking_up);                       // get the binary semaphore to wake up the mother eagle
	sem_getvalue(&full_pots, &number_of_full_pots);          // check if there are any full pots to eat from
	sem_getvalue(&non_empty_pots, &number_of_non_empty_pots);     // check if there are any baby eagles that are still eating from a pot (not finished eating yet)
	if (number_of_full_pots <= 0){
		while(number_of_non_empty_pots!=0){           // wait until all the eagles that started eating to finish
			sem_getvalue(&non_empty_pots, &number_of_non_empty_pots);
		}
		if (retired ==0){           // wake up the mother eagle if she has not retired yet,no synchronization needed since this section will be executed serially and mother eagle is the one that modifies the value , baby eagles only read the value
			
            babyWaker = id;         // fill babyWaker id with current id, no synchronization needed since this section will be executed serially and mother eagle does not modify the value it only reads it
			printf("Baby eagle %d sees all feeding pots are empty and wakes up the mother.\n",id);
			sem_post(&mother_st);   // wake up the mother, this section is serialized and only one baby eagle will wake up the mother if she has not retired yet and all the feeding pots are empty
			sem_wait(&can_eat);     // wait until mother eagle fills in all the feeding pots
		} else {                    // if mother eagle is retired and all the feeding pots are empty then exit
			printf("%*cBaby eagle %d sees that the mother has retired and exits.\n",id,' ',id);
			sem_post(&waking_up);   // release the semaphore to avoid deadlocks
			pthread_exit(0);	
		}
	}
	sem_wait(&full_pots);           // start eating from a pot
	sem_getvalue(&full_pots, &number_of_full_pots);  // get the value of the semaphore to indicate which pot to eat from
	sem_post(&waking_up);           // release the semaphore
	
	printf("%*cBaby eagle %d starts eating using feeding pot %d.\n",id,' ',id,number_of_full_pots+1);
}

// @brief finish_eating indicates that baby eagle finished eating from its feeding pot
// @param id long indicating the id of the caller thread
void finish_eating(long id){	
	printf("%*cBaby eagle %d finishes eating.\n",id,' ',id);
	sem_wait (&non_empty_pots);
}


// @brief baby_eagle is the main function for baby eagles, mainly eats if feeding pot is available otherwise wakes up the mother eagle to make food
// @param baby_id long indicating the id of the caller thread
void *baby_eagle(void * baby_id) {
	long id = (long) baby_id;
	printf("%*cBaby eagle %d started.\n",id,' ',id);
	while (1) {              
		sleep(2);           	 // play for a while     
		ready_to_eat(id);  		 // get hungry          
		sleep(2);            	 // eat for a while     
		finish_eating(id); 		 // finish eating       
	}
}

// @brief mother_goto_sleep makes mother eagle to sleep
void mother_goto_sleep(){
	printf("Mother eagle takes a nap.\n");
	sem_wait (&mother_st);
}

// @brief prepare_food  mother eagle prepares food by incrementing the full_pots and non_empty_pots to number_of_pots indicating that all the pots are full
// @param number_of_pots long indicating the number_of_pots
void prepare_food(long number_of_pots){
	printf("Mother eagle woken up by baby eagle %d.\n",babyWaker);
	int temp=0;
	while(temp<number_of_pots){
		sem_post (&full_pots);
		sem_post (&non_empty_pots);
		++temp;
	}
}

// @brief food_ready  mother eagle indicates that the food is ready by allowing the baby eagle to proceed
// @param t integer indicating the feeding iteration
void food_ready(int t){
	printf("Mother eagle says \"Feeding (%d)\".\n",t);
	sem_post(&can_eat);
}

// @brief baby_eamother_eaglegle is the main function for mother eagle, mainly eats if sleeps until a child wakes her up and then proceeds by preparing food and allowing the child eagles to eat 
// @param data struct of type input_data indicating the number_of_feedings and the number_of_pots 
void *mother_eagle(void *data) {
	printf("Mother eagle started.\n");
	long times = ((struct input_data*)data)->number_of_feedings;
	int i=1;
	while (i <= times) {                
		mother_goto_sleep();                                          // take a nap
		if (times == i) {retired = 1;}  
		prepare_food(((struct input_data*)data)->number_of_pots);     // prepare food 
		food_ready(i);    		                                      // make food available
		++i;                                                          // do something else  
	}
	printf("Mother eagle retires after serving %d feedings. Game ends!!!\n",times);
}
