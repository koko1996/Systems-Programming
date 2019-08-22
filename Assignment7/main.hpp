#ifndef GLOBAL_DATA_INCLUDED  // To make sure these are not declared more than once by including the header multiple times.
#define GLOBAL_DATA_INCLUDED

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <memory>
#include <unordered_map>
#include "buffer.hpp"
#include "helper.hpp"


/*
* Brief: map_thread_data_t is a struct that contains the data of the arguments
* that is going to be passed to the mapper threads
*/
struct map_thread_data_t {
  int tid;
  std::string file;
};
 
/*
* Brief: red_thread_data_t is a struct that contains the data of the arguments
* that is going to be passed to the reducer threads
*/
struct red_thread_data_t {
  int tid;
};

/*
* Brief: word_info is a struct that contains the file name and the line number
* that the word appear in
*/
struct word_info {
    std::string file_name;
    long line_number ;
};

/*
* Brief: data_t is a struct that contains the data produced by the mapper threads
*/
struct data_t {
    std::string word;
    word_info wi;
};

/*
* Brief: buffers is a vector of Buffer objects where each Buffer belongs
* to one of the reducers (consumers)
*/
std::vector<Buffer<data_t>> buffers;

/*
* Brief: sync_counter is counter object for the number of mapper threads
* currently active
*/
int sync_counter;
/*
* Brief: counter_lock is a mutex to provide synchronization for sync_counter
*/
pthread_mutex_t counter_lock;


#endif