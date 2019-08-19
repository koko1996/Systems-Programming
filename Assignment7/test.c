#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


int main(){
    char* msg;
    msg = (char*) malloc((strlen("fileNames[i]")+1)*sizeof(char));
    strcpy(msg, "fileNames[i]"); 
    printf("worked %s\n",msg);

}