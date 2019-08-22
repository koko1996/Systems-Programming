#ifndef HELPER_INCLUDED  // To make sure these are not declared more than once by including the header multiple times.
#define HELPER_INCLUDED

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <cstddef>
#include <sstream>
#include <fstream>
#include <utility>
#include <iostream>
#include <algorithm>

/* readCmdTokens tokenizes the passed parameter by space and returns an array of the tokens */
std::vector<std::string>  readCmdTokens(const std::string& cmd);

#endif 