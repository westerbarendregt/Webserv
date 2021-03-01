#include "Server.hpp"
#include "errno.h"
#include <string.h>

void	Server::respond(t_client &c) {
     char response[] = "HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: webserv\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nContent-Length: 4\r\nVary: Accept-Encoding\r\nContent-Type: text/plain\r\n\r\nOi\r\n";
	 if (send(c.m_socket, response, sizeof(response), 0) == -1)
		 std::cout<<"send: "<<strerror(errno)<<std::endl;
	 //check if send everything..
	 //reset client state
	 FD_CLR(c.m_socket, &this->m_write_all);
}
