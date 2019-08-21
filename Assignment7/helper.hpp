#ifndef HELPER_INCLUDED  // To make sure these are not declared more than once by including the header multiple times.
#define HELPER_INCLUDED

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <pthread.h>


int readCmdTokens(char* cmd, char** cmdTokens);

char readChar();

#endif 