#ifndef AUTHENTICATION_HPP
# define AUTHENTICATION_HPP

# include "RequestParser.hpp"
# include "Error.hpp"
# include "RequestHandler.hpp"

# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>

std::string         EncodePassword(std::string& password);
void                Authenticated(Client& c, RequestHandler& req);
void                AllowedMethods(Client& c, RequestHandler& req);
void                GetLanguage(Client& c, RequestHandler& req);



#endif
