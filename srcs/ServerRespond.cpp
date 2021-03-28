#include "Server.hpp"
#include "Error.hpp"
#include "errno.h"
#include <string.h>

void	Server::respond(int client_socket) {
	t_client *c;
	if ((c = this->getClient(client_socket)) == NULL) {
		throw serverError("removeClient: ", "trying to remove unexisting client");
	}
	this->respond(*c);
}

void	Server::respond(t_client &c) {
	std::cout<<"sending response"<<std::endl;
	std::cout << "RESPONSE:\n" << c.m_response_str.c_str() << std::endl;
	ssize_t	sent = 0;
	size_t	len = c.m_response_str.size();
	 if ((sent = send(c.m_socket, c.m_response_str.c_str(), len, 0)) == -1) {  //MSG_NOSIGNAL is not portable on MACOS, see main for global signal(SIGPIPE)
		 return closeClientConnection(c);
	 }
	 c.m_response_data.m_response_headers.clear();
	 c.m_response_str.erase(0, sent);
	 if (static_cast<size_t>(sent) == len) {
		 c.m_response_data.m_cgi_metadata_sent = 1;
		 if (c.m_request_data.m_cgi && !c.m_cgi_end_chunk)
			 return ;
	 	FD_CLR(c.m_socket, &this->m_write_all);
	 	FD_SET(c.m_socket, &this->m_read_all);
		c.reset();
		if (c.m_request_data.m_cgi)
			this->m_request_handler.m_cgi.reset(c);
	 }
}
