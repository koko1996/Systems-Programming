#ifndef THREAD_H
#define THREAD_H

#include <cstdio>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "boat-monitor.h"
#define NUMBER_OF_PASSENGERS_AT_A_TIME 3            // maximum number of passenger on the boat at a time

// main thread for cannibal
void *cannibal(void * );

// main thread for missionary
void *missionary(void * );

// main thread for the boat
void *boat(void *);


#endif