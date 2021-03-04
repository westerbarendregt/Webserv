#include "Server.hpp"
#include "Error.hpp"
#include "errno.h"
#include <string.h>

//void	Server::handleRequest(int client_socket) {
//	std::cout<<"handling request.."<<std::endl;
//	//get client
//	t_client *c = this->getClient(client_socket);
//	if (!c)
//		throw(serverError("getClient ", "client not registered"));
//	//treat request...
//}
