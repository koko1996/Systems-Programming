#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <cstdio>
#include <vector> 
#include <utility>      // std::pair, std::make_pair
#include <iostream>
#include "conditional_variable.hpp"


/*
* Brief: Buffer is a template class that provides a synchronized buffer
* where it is expected to have one consumer and multiple producers
* it is implemented as a monitor object
*/
template <typename T>
class Buffer {
    private:
        pthread_mutex_t monitor;    // mutex to provide mutual exclusion for the functions in this class
        pthread_cond_t  full;       // condition variable for full in consumer producer problem
        pthread_cond_t  empty;      // condition variable for empty in consumer producer problem

        int last;                   // index of the last inserted element into the container(buffer)
        int count;                  // number of valid inputs in the container(buffer)
        int SIZE;                   // size of the container(buffer)
        T *container;               // pointer to the contaier(buffer)

        void clean();
        void copyOther(Buffer<T> const& other);

    public:
        explicit Buffer(int size);                       // constructure
        ~Buffer();                                       // destructure
        Buffer(const Buffer<T>& s);                      // copy constructure
        Buffer& operator=(Buffer<T> const& other);       // overload assignment operator

        void append(T item);                             // add an element of type T to the container synchronously 
        T* remove(int *sync_counter);                    // remove an element of type T from the container synchronously
        bool isEmpty();                                  // helper function to check if the container is empty
        void wakeUpRemovingThreads();                    // helper function to wake up one sleeping thread that
                                                         // slept because there was no element to consume
};



/*
* Brief: Buffer constructor
*
* Param: size integer that represents the size of the container
* Returns: Buffer Object
*/
template <typename T>
Buffer<T>::Buffer(int size){
    pthread_mutex_init( &monitor, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    count=0;
    last=0;
    SIZE=size;
    container = new T[SIZE];
}

/*
* Brief: ~Buffer destructore
*
* Param: N/A
* Returns: N/A
*/
template <typename T>
Buffer<T>:: ~Buffer(){
    this->clean();
}

/*
* Brief: Buffer copy constructor (deep copy)
*
* Param: other Buffer object to copy from
* Returns: Buffer Object
*/
template <typename T>
Buffer<T>::Buffer(const Buffer<T>& other) {
    this->copyOther(other);
}

/*
* Brief: overload assignment operator (deep copy)
*
* Param: other Buffer object to copy from
* Returns: pointer to the current object
*/
template <typename T>
Buffer<T>& Buffer<T>::operator=(Buffer<T> const& other){
    if(this != &other) {
        this->clean();
        this->copyOther(other);
    }
    return *this;
}

/*
* Brief: helper function that copies deeply from passed Buffer object to this object
*
* Param: other Buffer object to copy from
* Returns: N/A
*/
template <typename T>
void Buffer<T>::copyOther(const Buffer<T>& other){
    this->last = other.last;
    this->count = other.count;
    this->SIZE = other.SIZE;
    this->monitor = other.monitor;
    this->full = other.full;
    this->empty = other.empty;
    this->container = new T[SIZE];
    for (int i=0;i<SIZE;++i){
        this->container[i] = other.container[i];
    }
}

/*
* Brief: helper function that cleans the current object
*
* Param: N/A
* Returns: N/A
*/
template <typename T>
void Buffer<T>::clean(){
    pthread_mutex_destroy(&monitor);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
    delete[] container;
}

/*
* Brief: append adds an element synchronously to the container
*
* Param: item of type T to add to the container
* Returns: N/A
*/
template <typename T>
void Buffer<T>::append (T item){
    pthread_mutex_lock(&monitor);      // mutex lock
        
    while (count == SIZE) {
        pthread_cond_wait(&empty, &monitor);
    }
    // printf("^^^^^^^^^^^^^^^^^^^^^^ 1 append %s -> count:%d size:%d last:%d ^^^^^^^^^^^^^^^^^^^^^^\n",(item.word).c_str(), count,SIZE,last);
    container[last] = item;
    last = (last + 1) % SIZE;
    count += 1;
    // printf("^^^^^^^^^^^^^^^^^^^^^^ 2 append %s -> count:%d size:%d last:%d ^^^^^^^^^^^^^^^^^^^^^^\n",(item.word).c_str(), count,SIZE,last);
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&monitor);     // mutex unlock
}

/*
* Brief: remove removes an element synchronously from the container
*
* Param: sync_counter integer pointer that points to the counter that 
* contains the number of active containers 
* Returns: returns pointer to item of type T that was removed from the 
* container if a non-consumed element exists otherwise returns nullptr
*/
template <typename T>
T* Buffer<T>::remove (int *sync_counter){
    T* item;      // pointer to the removed object
    // printf("***************** remove 1 -> sync_counter:%d *****************\n" ,*(sync_counter));     
    pthread_mutex_lock(&monitor);         // mutex lock
    
    while (count == 0) {
    
        // printf("***************** remove 2 -> sync_counter:%d *****************\n" ,*(sync_counter));     
    
        if (*(sync_counter) == 0){
            pthread_mutex_unlock(&monitor);
    
            // printf("***************** remove return nullptr *****************\n");     
    
            return nullptr;
        }
        pthread_cond_wait(&full, &monitor); // mutex unlock
    }
    
    // printf("***************** remove 3 -> count:%d size:%d last:%d *****************\n" ,count,SIZE,last);   

    item = new T(container[(SIZE + (last-count)%SIZE ) % SIZE]);
    count -= 1;
    
    // printf("***************** remove 4 %s -> count:%d size:%d last:%d *****************\n", (item->word).c_str() ,count,SIZE,last);     
    
    pthread_cond_signal(&empty);

    pthread_mutex_unlock(&monitor);         // mutex unlock
    
    return item;
}

/*
* Brief: wakeUpRemovingThreads helper function to wake up one sleeping thread that
* slept because there was no element to consume in the container
*
* Param: N/A
* Returns: N/A
*/
template <typename T>
void Buffer<T>::wakeUpRemovingThreads(){
    // printf("***************** broad Cast -> count:%d size:%d last:%d *****************\n",count,SIZE,last);     
    pthread_cond_signal(&full);
}

/*
* Brief: isEmpty is a helper function to check if the container has any non-consumed elements
*
* Param: N/A
* Returns: true if the container is empty and false otherwise
*/
template <typename T>
bool Buffer<T>::isEmpty(){
    return (count==0);
}


#endif

