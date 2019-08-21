#include "main.hpp"

/* thread function */
void *mapper_func(void *arg) {

    map_thread_data_t *data = (map_thread_data_t *)arg;

    // printf("^^^^^^^^^^^^^^^^^^^^^^ mapper_func thread id: %d, file name:%s ^^^^^^^^^^^^^^^^^^^^^^\n", data->tid,(data->file).c_str());

    FILE* file = fopen((data->file).c_str(), "r"); /* should check the result */
    char line[256];
    long line_number = 0;
    while (fgets(line, sizeof(line), file)) {
        char* pos;
        if ((pos=strchr(line, '\n')) != NULL){
            *pos = '\0';
        }
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        data_t pd;
        pd.word = line;
        pd.wi.file_name=(data->file);
        pd.wi.line_number = line_number;

        buffers.append(pd);
        
        ++line_number;
    }
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */
    fclose(file);

    pthread_mutex_lock(&counter_lock);
    --sync_counter;
    pthread_mutex_unlock(&counter_lock);
    if (sync_counter == 0){
        buffers.wakeUpRemovingThreads();
    }
    pthread_exit(NULL);
}

/* thread function */
void *reducer_func(void *arg) {
    std::unordered_map<std::string, std::vector<word_info>> inverted_index; 
    red_thread_data_t *data = (red_thread_data_t *)arg;
    
    while  (!(sync_counter == 0 && buffers.isEmpty() )){
        printf("***************** reducer_func thread id: %d %d %d \n", data->tid, sync_counter,buffers.isEmpty()); 
        data_t* pd = buffers.remove(&sync_counter);
        if (pd == nullptr){
            printf("***************** reducer_func nullptr thread id: %d %d\n", data->tid, sync_counter); 
        } else {
            printf("***************** reducer_func thread id: %d %s\n", data->tid, (pd->word).c_str()); 
            if (inverted_index.find(pd->word) == inverted_index.end()) { // not found
                std::vector<word_info> vect;
                vect.push_back(pd->wi);
                inverted_index.insert(std::make_pair(pd->word,vect));
            } else {
                inverted_index[pd->word].push_back(pd->wi);
            }
            delete pd;
        }
    }
    printf("***************** reducer_func thread id: %d DONE\n", data->tid); 
    for (std::unordered_map<std::string, std::vector<word_info>>::iterator it = inverted_index.begin(); it != inverted_index.end(); ++it ){
        std::string value;
        for(std::vector<word_info>::iterator vit = (it->second).begin(); vit != (it->second).end() ; ++vit){
            value += "("+(vit->file_name + ":") + std::to_string(vit->line_number) + ") ";
        }
        printf("@@@@@ Results thread id: %d key:%s value:%s \n", data->tid,(it->first).c_str(),value.c_str());
    }
    pthread_exit(NULL);
}

// main method - program entry point
int main() {
    char cmd[81]; // array of chars (a string)
    char* fileNames[20]; // array of strings
    int FILES;  // number of times to execute command
    int MAPPERS; // number of times to execute command
    int REDUCERS; // number of times to execute command

    // begin parsing code - do not modify
    printf("Files> ");
    fgets(cmd, sizeof(cmd), stdin);
    FILES = readCmdTokens(cmd, fileNames);
    
    do {
        printf("  Mappers> ");
        MAPPERS = readChar() - '0';
    } while (MAPPERS <= 0 || MAPPERS != FILES);
    do {
        printf("  Reducers> ");
        REDUCERS = readChar() - '0';
    } while (REDUCERS <= 0);

    
    for (int i=0;i<FILES;i++){
        printf("filename:%s\n",fileNames[i]); 
    }
    // end parsing code


    if (pthread_mutex_init(&counter_lock, NULL) != 0) {
        printf("\n mutex init failed\n");
        return 1;
    }
    sync_counter = MAPPERS;

    for (int i = 0; i < REDUCERS; ++i) {
        bufferss.push_back(Buffer<data_t>());
    }

    // Init Mappers
    /* create a map_thread_data_t argument array */
    map_thread_data_t mapper_data[MAPPERS];
    pthread_t mapper_thr[MAPPERS];
    int mrc;

    /* create threads */
    for (int i = 0; i < MAPPERS; ++i) {
        mapper_data[i].tid = i;
        mapper_data[i].file = fileNames[i];

        if ((mrc = pthread_create(&mapper_thr[i], NULL, mapper_func, &mapper_data[i]))) {
            fprintf(stderr, "error: pthread_create, retrun code: %d\n", mrc);
            return EXIT_FAILURE;
        }
    }

    // Init Reducers
    /* create a map_thread_data_t argument array */
    red_thread_data_t reducer_data[REDUCERS];
    pthread_t reducer_thr[REDUCERS];
    int rrc;
    
    /* create threads */
    for (int i = 0; i < REDUCERS; ++i) {
        reducer_data[i].tid = i;

        if ((rrc = pthread_create(&reducer_thr[i], NULL, reducer_func, &reducer_data[i]))) {
            fprintf(stderr, "error: pthread_create, return code: %d\n", rrc);
            return EXIT_FAILURE;
        }
    }

    /* block until all threads complete */
    for (int i = 0; i < MAPPERS; ++i) {
        pthread_join(mapper_thr[i], NULL);
    }
    
    /* block until all threads complete */
    for (int i = 0; i < REDUCERS; ++i) {
        pthread_join(reducer_thr[i], NULL);
    }


    pthread_mutex_destroy(&counter_lock);
}

