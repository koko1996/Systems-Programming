#ifndef GLOBAL_DATA_INCLUDED  // To make sure these are not declared more than once by including the header multiple times.
#define GLOBAL_DATA_INCLUDED

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <unordered_map>
#include "buffer.hpp"
#include "helper.hpp"


struct map_thread_data_t {
  int tid;
  std::string file;
};
 
struct red_thread_data_t {
  int tid;
};

struct word_info {
    std::string file_name;
    long line_number ;
};

struct data_t {
    std::string word;
    word_info wi;
};

std::vector<Buffer<data_t>> bufferss;
Buffer<data_t> buffers;

int sync_counter;
pthread_mutex_t counter_lock;




#endif