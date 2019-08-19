#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "map.c"

#define TRUE 1
#define FALSE 0


// tokenize the command string into arguments - do not modify
int readCmdTokens(char* cmd, char** cmdTokens) {
    cmd[strlen(cmd) - 1] = '\0'; // drop trailing newline
    int i = 0;
    cmdTokens[i] = strtok(cmd, " "); // tokenize on spaces
    while (cmdTokens[i++] && i < sizeof(cmdTokens)) {
        cmdTokens[i] = strtok(NULL, " ");
    }
    return (i-1);
}

// read one character of input, then discard up to the newline - do not modify
char readChar() {
    char c = getchar();
    while (getchar() != '\n');
    return c;
}

// main method - program entry point
int main() {
    char cmd[81]; // array of chars (a string)
    char* fileNames[20]; // array of strings
    int FILES;  // number of times to execute command
    int MAPPERS; // number of times to execute command
    int REDUCERS; // number of times to execute command
    int i=0;

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

    
    for (i=0;i<FILES;i++){
        printf("filename:%s\n",fileNames[i]); 
    }
    // end parsing code

    // Mappers
    pthread_t mapper_thr[MAPPERS];
    int rc;
    /* create a thread_data_t argument array */
    thread_data_t mapper_data[MAPPERS];
    
    /* create threads */
    for (i = 0; i < MAPPERS; ++i) {
        mapper_data[i].tid = i;
        mapper_data[i].file = (char*) malloc((strlen(fileNames[i])+1)*sizeof(char));
        strcpy(mapper_data[i].file,fileNames[i] );

        if ((rc = pthread_create(&mapper_thr[i], NULL, mapper_func, &mapper_data[i]))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
    }
    /* block until all threads complete */
    for (i = 0; i < MAPPERS; ++i) {
        pthread_join(mapper_thr[i], NULL);
    }
    
    /* Clean up */
    for (i = 0; i < MAPPERS; ++i) {
        free(mapper_data[i].file);
    }
      
    
}

