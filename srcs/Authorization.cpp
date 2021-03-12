#include "something.hpp"


std::string         EncodePassword(std::string& password)
{
    std::string encoded;
    int index = 0;
    for (int i = 0; password[i]; ++i){
        if (password[i] > 100){
            encoded[index] = password[i] - 10; 
            index++;
            encoded[index] = password[i] - 22;
            index++;
        }
        else {
            encoded[index] = password[i] + 10; 
            index++;
            encoded[index] = password[i] + 22;
            index++;
        }
    }
    return encoded;
}

bool                Authenticate(Client& c)
{
    if (c.)
}
