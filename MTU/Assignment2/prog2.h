#ifndef PROG_H
#define PROG_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

struct input_data {				// struct to hold shared config data
	int number_of_pots;
	long number_of_feedings;
};


sem_t mother_st; 		// binary semaphore representing mother eagle's state (sleep or awake)
sem_t can_eat; 			// binary semaphore representing if baby eagles can eat or they must wait until mother fills up all the pots
sem_t full_pots; 		// number of pots that are full (no baby eagle touched it yet)
sem_t non_empty_pots; 	// number of pots that are not full (baby eagle is occupying it)

sem_t waking_up; 		// binary semaphore representing the authorization for waking up the mother
long babyWaker;			// long that will hold the id of the baby eagle that wakes up the mother eagle

int retired;			// 1 if mother eagle is retired 0 otherwise


void ready_to_eat(long );		// baby eagle is ready to eat

void finish_eating(long );		// baby eagle is finishes eating it's feeding pot

void *baby_eagle(void* ) ;		// baby eagle main function

void mother_goto_sleep();		// mother eagle goes to sleep

void prepare_food(long );		// mother eagle starts preparing food

void food_ready(int );			// mother eagle finishes preparing food

void *mother_eagle(void*) ;		// mother eagle main function


#endif