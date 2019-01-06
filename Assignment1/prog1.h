#ifndef PROG_H
#define PROG_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


int *values;                        // Global array that holds the input values (so that all the threads have access to it)
int *input;                         // Global array that holds the comparison values (so that all the threads have access to it)


// comparision struct
struct compare {
   long   first;
   long   second;
};

//@brief each thread initializes 1 to the values array at index passed as an argument
//@param id_ptr a pointer that points to long value representing the index to be set
void *init_array(void *id_ptr) {
	long id = (long) id_ptr;
	values[id]=1;	
}

//@brief each thread initializes compares the two values from the array input and sets 0 at the index of the minimum value in the values array 
//@param cmp_ptr a pointer that points to compare struct and first member of the struct is inialized to the first element to be compared and second to second
void *compare_array(void *cmp_ptr) {
	struct compare * cmp = (struct compare *) cmp_ptr;

	if (input[cmp->first] >  input[cmp->second]){	
		values[cmp->second]=0;	
	    printf("Thread T(%d,%d) compares x[%d] = %d and x[%d] = %d and writes %d into w[%d]\n", cmp->first, cmp->second, cmp->first, input[cmp->first], cmp->second, input[cmp->second], 0 , cmp->second);
	} else if (input[cmp->first] < input[cmp->second]){
		values[cmp->first]=0;	
	    printf("Thread T(%d,%d) compares x[%d] = %d and x[%d] = %d and writes %d into w[%d]\n", cmp->first, cmp->second, cmp->first, input[cmp->first], cmp->second, input[cmp->second], 0 , cmp->first);
	}
}


//@brief each thread check the value at index passed as an argument, and prints out the value at the same index in the values array (user input) which represents the maximum number
//@param id_ptr a pointer that points to long value representing the index to be checked
void *max_array(void *id_ptr) {
	long id = (long) id_ptr;
	if (values[id]==1) {
		printf("Maximum                = %d\n", input[id]);
		printf("Location               = %d\n", id);
	}
}


#endif