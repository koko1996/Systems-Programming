#include "helper.hpp"

/*
* Brief: readCmdTokens tokenizes the parameter by space and returns an array of the 
* tokens
*
* Param: cmd is const string to parse
* Returns: vector of strings where each element is a token in cmd (parameter passed)
*/
std::vector<std::string> readCmdTokens(const std::string& cmd) {
    char delimeter= ' ';
    std::stringstream ss(cmd);
    std::string item;
    std::vector<std::string> splitted_strings;
    while (std::getline(ss, item, delimeter))
    {
        if (!std::all_of(item.begin(),item.end(),isspace)){ // check if it has more than just spaces
            // trim the resulting string then add it to the resulting vector
            splitted_strings.push_back(item);
        }
        
    }
    return splitted_strings;
}
