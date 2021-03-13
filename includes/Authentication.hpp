#ifndef AUTHENTICATION_HPP
# define AUTHENTICATION_HPP

# include "RequestParser.hpp"

std::string         EncodePassword(std::string& password);
bool                Authenticated(Client& c);



#endif
