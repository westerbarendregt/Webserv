#include "Server.hpp"
#include "Error.hpp"
#include "errno.h"
#include <string.h>

void	Server::respond(int client_socket) {
	std::cout<<"sending response"<<std::endl;
	t_client *c;

	if ((c = this->getClient(client_socket)) == NULL) {
		throw serverError("removeClient: ", "trying to remove unexisting client");
	}
	 if (send(c->m_socket, c->m_response_str.c_str(), c->m_response_str.size(), 0) == -1)
		 std::cout<<"send: "<<strerror(errno)<<std::endl;
	 FD_CLR(c->m_socket, &this->m_write_all);
}
