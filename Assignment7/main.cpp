#include "main.hpp"

/*
* Brief: mapper_func is mapper thread function t
*
* Param: arg is a pointer of type map_thread_data_t
* Returns: N/A
*/
void *mapper_func(void *arg) {

    map_thread_data_t *data = (map_thread_data_t *)arg; // cast the argument

    // printf("^^^^^^^^^^^^^^^^^^^^^^ mapper_func thread id: %d, file name:%s ^^^^^^^^^^^^^^^^^^^^^^\n", data->tid,(data->file).c_str());

    FILE* file = fopen((data->file).c_str(), "r");  // open the file
    char line[256];                                 // buffer to read
    long line_number = 0;                           // line number
    while (fgets(line, sizeof(line), file)) {       
        char* pos;                                  // remove trailing new line 
        if ((pos=strchr(line, '\n')) != NULL){
            *pos = '\0';
        }

        data_t pd;                                  // produce the object
        pd.word = line;
        pd.wi.file_name=(data->file);
        pd.wi.line_number = line_number;
        std::size_t h1 = std::hash<std::string>{}(pd.word); 
        std::size_t index = h1 % buffers.size();    // find the corresponding reducer
        buffers[index].append(pd);                  // append the object to the reducer's buffer
        
        ++line_number;
    }

    fclose(file);

    // decrement the number of active mapper threads since this thread is done
    pthread_mutex_lock(&counter_lock);
    --sync_counter;
    // if this is the last mapper thread than wake up all of the reducer threads 
    // the reason for this is because some of the reducers might be asleep because 
    // their Buffer is empty and there was at least one mapper when they tried to  
    // remove an object from Buffer. Since it is possible that the last mapper thread
    // did not put the data in the sleeping reducer thread, this reducer thread was not
    // woken up. Hence, we need to wake up all the reducer threads to inform that the mapper
    // threads are done.
    if (sync_counter == 0){
        for(std::vector<Buffer<data_t>>::iterator it=buffers.begin(); it!=buffers.end(); ++it ){
            it->wakeUpRemovingThreads();
        }
    }
    pthread_mutex_unlock(&counter_lock);

    pthread_exit(NULL);
}

/*
* Brief: reducer_func is reducer thread function t
*
* Param: arg is a pointer of type red_thread_data_t
* Returns: N/A
*/
void *reducer_func(void *arg) {

    std::unordered_map<std::string, std::vector<word_info>> inverted_index;  // hash map to store the inverted index
    
    red_thread_data_t *data = (red_thread_data_t *)arg;     // cast the argument
    
    /* loop while there is at leas one mapper thread or this reducer thread's buffer is not empty
        isEmpty() function is not synchronized however this is not an issue. Since, there are only four cases:
    1) (sync_counter == 0 && buffers[data->tid].isEmpty()== true ) then this reducer thread is done based 
    on the program specification and the value of buffers[data->tid].isEmpty() can't change anymore (no race condition)
    2) (sync_counter > 0 && buffers[data->tid].isEmpty()== true ) then this reducer thread needs to try to
    consume at least one more time and 
    3) (sync_counter == 0 && buffers[data->tid].isEmpty()== false ) 
    4) (sync_counter > 0 && buffers[data->tid].isEmpty()== false ) 
    */
    while  (!(sync_counter == 0 && buffers[data->tid].isEmpty() )){
        // printf("***************** reducer_func thread id: %d %d %d \n", data->tid, sync_counter,buffers[data->tid].isEmpty()); 
        
        std::unique_ptr<data_t> pd (buffers[data->tid].remove(&sync_counter)); // remove an element from the Buffer
        if (pd != nullptr){     
            // printf("***************** reducer_func thread id: %d %s\n", data->tid, (pd->word).c_str()); 
            // add the consumed data to the inverted_index
            if (inverted_index.find(pd->word) == inverted_index.end()) { // not found
                std::vector<word_info> vect;
                vect.push_back(pd->wi);
                inverted_index.insert(std::make_pair(pd->word,vect));
            } else {    // found
                inverted_index[pd->word].push_back(pd->wi);
            }
        }
    }
    
    // printing the inverted_index
    // printf("***************** reducer_func thread id: %d DONE\n", data->tid); 
    for (std::unordered_map<std::string, std::vector<word_info>>::iterator it = inverted_index.begin(); it != inverted_index.end(); ++it ){
        std::string value;
        for(std::vector<word_info>::iterator vit = (it->second).begin(); vit != (it->second).end() ; ++vit){
            value += "("+(vit->file_name + ":") + std::to_string(vit->line_number) + ") ";
        }
        printf("%s -> %s \n",(it->first).c_str(),value.c_str());
    }
    pthread_exit(NULL);
}

// main method - program entry point
int main() {
    std::string cmd;    // string to hold the input line
    int MAPPERS;        // number of mapper threads
    int REDUCERS;       // number of mapper reducers
    std::vector<std::string> fileNames; // vector to hold the file names 
    
    ///////////////////////////////////////////////////////
    /////////////////// Parsing Section ///////////////////
    ///////////////////////////////////////////////////////

    printf("Files> ");
    getline( std::cin, cmd);
    fileNames =readCmdTokens(cmd);
    printf("Number Of Files:%d\n",fileNames.size());     

    do {
        std::cout <<"  Mappers> ";
        std::cin >> MAPPERS;
    } while (MAPPERS <= 0 || MAPPERS != fileNames.size());
    do {
        std::cout <<"  Reducers> ";
        std::cin >> REDUCERS;
    } while (REDUCERS <= 0);


    // fileNames.push_back("files/file1.txt");
    // fileNames.push_back("files/file2.txt");
    // fileNames.push_back("files/file3.txt");
    // fileNames.push_back("files/file4.txt");
    // // fileNames.push_back("files/tmp.txt");
    // MAPPERS = fileNames.size();
    // REDUCERS = 10;

    ///////////////////////////////////////////////////////
    //////////////// Intialization Section ////////////////
    ///////////////////////////////////////////////////////

    if (pthread_mutex_init(&counter_lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }
    
    sync_counter = MAPPERS;

    for (int i = 0; i < REDUCERS; ++i) {
        buffers.push_back(Buffer<data_t>(10));
    }

    // Initialize Mapper threads
    // create a map_thread_data_t argument array
    map_thread_data_t mapper_data[MAPPERS];
    pthread_t mapper_thr[MAPPERS];
    int mrc;

    // create mapper threads 
    for (int i = 0; i < MAPPERS; ++i) {
        mapper_data[i].tid = i;
        mapper_data[i].file = fileNames[i];

        if ((mrc = pthread_create(&mapper_thr[i], NULL, mapper_func, &mapper_data[i]))) {
            fprintf(stderr, "error: pthread_create, retrun code: %d\n", mrc);
            return EXIT_FAILURE;
        }
    }

    // Init Reducer threads
    // create a map_thread_data_t argument array 
    red_thread_data_t reducer_data[REDUCERS];
    pthread_t reducer_thr[REDUCERS];
    int rrc;
    
    // create reducer threads 
    for (int i = 0; i < REDUCERS; ++i) {
        reducer_data[i].tid = i;

        if ((rrc = pthread_create(&reducer_thr[i], NULL, reducer_func, &reducer_data[i]))) {
            fprintf(stderr, "error: pthread_create, return code: %d\n", rrc);
            return EXIT_FAILURE;
        }
    }


    ///////////////////////////////////////////////////////
    /////////////////// Clean up Section //////////////////
    ///////////////////////////////////////////////////////

    /* block until all mapper threads complete */
    for (int i = 0; i < MAPPERS; ++i) {
        pthread_join(mapper_thr[i], NULL);
    }
    
    /* block until all reducer threads complete */
    for (int i = 0; i < REDUCERS; ++i) {
        pthread_join(reducer_thr[i], NULL);
    }

    pthread_mutex_destroy(&counter_lock);
}

