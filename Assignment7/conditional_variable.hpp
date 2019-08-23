#ifndef CONDITIONAL_VARIABLE_H
#define CONDITIONAL_VARIABLE_H

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <cstdio>
#include <vector> 
#include <utility>      // std::pair, std::make_pair
#include <iostream>

class ConditionalVariable {
   private:
        sem_t semaphore;

        void clean();
        void copyOther(ConditionalVariable const& other);
    public:
        explicit ConditionalVariable();                       // constructure
        ~ConditionalVariable();
        ConditionalVariable(const ConditionalVariable& other);                      // copy constructure
        ConditionalVariable& operator=(ConditionalVariable const& other);       // overload assignment operator

        void wait(pthread_mutex_t* mutex);
        void signal();
        void signalWithState();
};

ConditionalVariable::ConditionalVariable(){
    sem_init(&semaphore, 0, 1);
}

ConditionalVariable::~ConditionalVariable(){
    this->clean();
}

ConditionalVariable::ConditionalVariable(const ConditionalVariable& other){
    this->copyOther(other);
}

ConditionalVariable& ConditionalVariable::operator=(ConditionalVariable const& other){
    if(this != &other) {
        this->clean();
        this->copyOther(other);
    }
    return *this;
}

void ConditionalVariable::clean(){
    sem_destroy(&semaphore);
}

void ConditionalVariable::copyOther(ConditionalVariable const& other){
    this->semaphore = other.semaphore;
}

void ConditionalVariable::wait(pthread_mutex_t* mutex){
    pthread_mutex_unlock(mutex);        // Release lock associated with this CV
    sem_wait(&semaphore); 
    pthread_mutex_lock(mutex);        // Reacquire lock associated with this CV
}

void ConditionalVariable::signal(){
    int value;
    sem_getvalue(&semaphore, &value);
    if (value < 0){
        sem_post(&semaphore); 
    }
}

void ConditionalVariable::signalWithState(){
    sem_post(&semaphore); 
}

#endif