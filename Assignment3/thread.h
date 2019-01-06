#ifndef THREAD_H
#define THREAD_H

#include <cstdio>
#include <stdlib.h>
#include <pthread.h>

void *cannibal(void * );

void *missionary(void * );

void *boat(void *);

#endif