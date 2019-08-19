// closh.c - CS 377, Fall 2012
// Koko Nanahji

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE 0

// tokenize the command string into arguments - do not modify
void readCmdTokens(char* cmd, char** cmdTokens) {
    cmd[strlen(cmd) - 1] = '\0'; // drop trailing newline
    int i = 0;
    cmdTokens[i] = strtok(cmd, " "); // tokenize on spaces
    while (cmdTokens[i++] && i < sizeof(cmdTokens)) {
        cmdTokens[i] = strtok(NULL, " ");
    }
}

// read one character of input, then discard up to the newline - do not modify
char readChar() {
    char c = getchar();
    while (getchar() != '\n');
    return c;
}

// brief: checks if all the child process are alive
// p_pids: pointer to an array of pids that will be checked for aliveness
// count: number of elements in the array p_pids
// return: 1 if all process ids specified in the array p_pids are no alive and 0 otherwise
int noChildAlive(pid_t* p_pids,int count){
    int result;
    int status;
    int ans = 1;
    int i=0;
    for (i=0;i<count;i++){
        result = waitpid(p_pids[i], &status, WNOHANG);
        if (result == 0) {
            ans = 0;
        }
    }
    return ans;
}

// brief: kill all the child process that are alive
// p_pids: pointer to an array of pids that will be checked for aliveness
// count: number of elements in the array p_pids
// return: N/A
void killAllChilderen(pid_t* p_pids, int count){
    int i=0;
    for (i=0;i<count;i++){
        kill(p_pids[i], SIGKILL);
    }
}


// main method - program entry point
int main() {
    char cmd[81]; // array of chars (a string)
    char* cmdTokens[20]; // array of strings
    int count; // number of times to execute command
    int parallel; // whether to run in parallel or sequentially
    int timeout; // max seconds to run set of commands (parallel) or each command (sequentially)
    
    while (TRUE) { // main shell input loop    
        // begin parsing code - do not modify
        printf("closh> ");
        fgets(cmd, sizeof(cmd), stdin);
        if (cmd[0] == '\n') continue;
        readCmdTokens(cmd, cmdTokens);
        do {
            printf("  count> ");
            count = readChar() - '0';
        } while (count <= 0 || count > 9);
        
        printf("  [p]arallel or [s]equential> ");
        parallel = (readChar() == 'p') ? TRUE : FALSE;
        do {
            printf("  timeout> ");
            timeout = readChar() - '0';
        } while (timeout < 0 || timeout > 9);
        // end parsing code

        pid_t pid;     
        pid_t wpid;
        pid_t p_pids[count];    // array to save the pids of child processes 
        int result;
        int status;
        int count_t = count; 

        while (count_t > 0){
            pid = fork();
            if (pid == 0) { // child process
                // just executes the given command once - REPLACE THIS CODE WITH YOUR OWN
                execvp(cmdTokens[0], cmdTokens); // replaces the current process with the given program
                // doesn't return unless the calling failed
                printf("Can't execute %s\n", cmdTokens[0]); // only reached if running the program failed
                exit(1);  
            } else { // parent process
                if (parallel == FALSE){ // sequential case
                    if (timeout != 0 ){ // check if the user specified a timeout
                        int timeout_t = timeout;
                        int stillAlive = 1;
                        while(timeout_t > 0 && stillAlive == 1){ // wait until the timeout time is up or the child terminated
                            result = waitpid(pid, &status, WNOHANG); // check if the child is still alive
                            if (result == 0) {  //  sleep for a second if the child is still alive
                                sleep(1);
                            } else if (result == -1) {
                                // Error 
                            } else {
                                stillAlive = 0;
                            }
                            timeout_t--;
                        }
                        
                        result = waitpid(pid, &status, WNOHANG);
                        if (result == 0) {  // check if the child is still alive and kill it if it is still alive 
                            kill(pid, SIGKILL);
                            waitpid(pid, &status, 0);
                        } 
                    } else {    // no timeout was specified so wait until child finishes 
                        wait(&status);
                    }
                } else {    // save the pid of the child process
                    *(p_pids + count_t - 1) = pid;
                }
                --count_t;
            }
        }
        if (parallel == TRUE ){ // parallel case 
            if (timeout != 0 ){ // check if the user specified a timeout
                int timeout_t = timeout;
                int stillAlive = 1;
                while(timeout_t > 0 && stillAlive == 1){    // wait until the timeout time is up or all the childeren are terminated
                    result = noChildAlive(p_pids,count);    // check if any child is alive
                    if (result == 0) {  //  sleep for a second if the child is still alive
                        sleep(1);       
                    } else if (result == -1) {
                        // Error 
                    } else {
                        stillAlive = 0;
                    }
                    timeout_t--;
                }
                result = noChildAlive(p_pids,count);
                if (result == 0) { // check if there is at least one child did not die in the loop Timeout
                    killAllChilderen(p_pids,count); // make sure all the childeren are dead
                    while ((wpid = wait(&status)) > 0); // clean up the return values 
                } 
        } else {  // no timout was specified so wait until all childeren are done
                while ((wpid = wait(&status)) > 0);
            }
            
        }
    }
}

