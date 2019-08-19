#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/* thread function */
void *mapper_func(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;

    printf("hello from mapper_func, thread id: %d, file name:%s\n", data->tid,(data->file));
    

    FILE* file = fopen(data->file, "r"); /* should check the result */
    char line[256];
    int line_numer = 0;
    while (fgets(line, sizeof(line), file)) {
        char* pos;
        if ((pos=strchr(line, '\n')) != NULL){
            *pos = '\0';
        }
        /* note that fgets don't strip the terminating \n, checking its
           presence would allow to handle lines longer that sizeof(line) */
        printf("%s -> %s:%d\n", line,(data->file),line_numer); 
        ++line_numer;
    }
    /* may check feof here to make a difference between eof and io failure -- network
       timeout for instance */

    fclose(file);

    pthread_exit(NULL);
}