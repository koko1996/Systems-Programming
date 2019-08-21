#ifndef BUFFER_H
#define BUFFER_H

#include <cstdio>
#include <iostream>
#include <stdlib.h>
#include <vector> 
#include <utility>      // std::pair, std::make_pair
#include <pthread.h>
#include <semaphore.h>

template <typename T>
class Buffer {

private:
    pthread_mutex_t monitor;   // mutex to provide mutual exclusion for the functions in this class
    pthread_cond_t  full;      // condition variable for 
    pthread_cond_t  empty;      // condition variable for 

    int last;
    int count;
    int SIZE;
    T *container;

    void clean();
    void copyOther(Buffer<T> const& other);
public:
    Buffer();                       // constructure
    ~Buffer();                       // constructure
    Buffer(const Buffer<T>& s);                                        // copy constructure
    Buffer& operator=(Buffer<T> const& other);                         // assignment operator

    void append(T item);     
    T* remove(int *sync_counter); 
    bool isEmpty();
    void wakeUpRemovingThreads();
};


template <typename T>
Buffer<T>::Buffer(){
    pthread_mutex_init( &monitor, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    count=0;
    last=0;
    SIZE=10;
    container = new T[SIZE];
}

template <typename T>
Buffer<T>:: ~Buffer(){
    this->clean();
}

template <typename T>
Buffer<T>::Buffer(const Buffer<T>& other) {
    this->copyOther(other);
}

template <typename T>
Buffer<T>& Buffer<T>::operator=(Buffer<T> const& other){
    if(this != &other) {
        this->clean();
        this->copyOther(other);
    }
    return *this;
}

template <typename T>
void Buffer<T>::copyOther(const Buffer<T>& other){
    this->last = other.last;
    this->count = other.count;
    this->SIZE = other.SIZE;
    printf("copyOther oh boi");
}

template <typename T>
void Buffer<T>::clean(){
    pthread_mutex_destroy(&monitor);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
    delete[] container;
}

template <typename T>
void Buffer<T>::append (T item){
    pthread_mutex_lock(&monitor);         // mutex lock
        
    while (count == SIZE) {
        pthread_cond_wait(&empty, &monitor);
    }
    printf("^^^^^^^^^^^^^^^^^^^^^^ 1 append %s -> count:%d size:%d last:%d ^^^^^^^^^^^^^^^^^^^^^^\n",(item.word).c_str(), count,SIZE,last);
    container[last] = item;
    last = (last + 1) % SIZE;
    count += 1;
    printf("^^^^^^^^^^^^^^^^^^^^^^ 2 append %s -> count:%d size:%d last:%d ^^^^^^^^^^^^^^^^^^^^^^\n",(item.word).c_str(), count,SIZE,last);
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&monitor);
}

template <typename T>
T* Buffer<T>::remove (int *sync_counter){
    T* item;
    printf("***************** remove 1 -> sync_counter:%d *****************\n" ,*(sync_counter));     
    pthread_mutex_lock(&monitor);         // mutex lock
    
    while (count == 0) {
        printf("***************** remove 2 -> sync_counter:%d *****************\n" ,*(sync_counter));     
        if (*(sync_counter) == 0){
            pthread_mutex_unlock(&monitor);
            printf("***************** remove return nullptr *****************\n");     
            return nullptr;
        }
        pthread_cond_wait(&full, &monitor);
    }
    printf("***************** remove 3 -> count:%d size:%d last:%d *****************\n" ,count,SIZE,last);     
    item = &(container[(SIZE + (last-count)%SIZE ) % SIZE]);
    count -= 1;
    printf("***************** remove 4 %s -> count:%d size:%d last:%d *****************\n", (item->word).c_str() ,count,SIZE,last);     
    pthread_cond_signal(&empty);

    pthread_mutex_unlock(&monitor);
    
    return new T(*item);
}

template <typename T>
void Buffer<T>::wakeUpRemovingThreads(){
    printf("***************** broad Cast -> count:%d size:%d last:%d *****************\n",count,SIZE,last);     
    pthread_cond_broadcast(&full);
}


template <typename T>
bool Buffer<T>::isEmpty(){
    return (count==0);
}


#endif

