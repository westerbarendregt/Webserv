#include "Server.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <exception>
#include <fcntl.h>
#include "Error.hpp"
#include "WebServer.hpp"
#include "Logger.hpp"

ssize_t	Server::receive(t_client *c) {
	if (!c)
		throw(serverError("getClient ", "client not registered"));

	char buf[RECV_BUF_SIZE];
	ssize_t nbytes = recv(c->m_socket, buf, RECV_BUF_SIZE - 1, 0);
	if (nbytes == -1){
		Logger::Log()<<"recv: "<<strerror(errno)<<std::endl;
		return INVALID;
	}
	buf[nbytes] = '\0';
	// Logger::Log() << std::endl <<  "      RECV:    " << std::endl << buf << std::endl;
	const char *to_append = buf;
	c->m_request_str.append(to_append);
	return nbytes;
}
