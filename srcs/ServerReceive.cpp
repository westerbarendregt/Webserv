#include "Server.hpp"
#include "RequestParser.hpp"

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

int	Server::receive(t_client *c) {
	char buf[1000]; // can have buf elsewhere
	memset(buf, 0, sizeof(buf));//fill
	if (!c)
		throw(serverError("getClient ", "client not registered"));
	int	nbytes = recv(c->m_socket, buf, sizeof(buf), 0);
	const char *to_append = buf;
	c->m_request_str.append(to_append);
	if (nbytes <= 0){ // should we handle closing a connection in the ServerRun loop
		if (nbytes == 0)
			std::cout<<"closing connection "<<c->m_socket<<std::endl; //log
		else
			std::cout<<"recv: "<<strerror(errno)<<std::endl; //log
		::close(c->m_socket);
		FD_CLR(c->m_socket, &this->m_read_all);
		this->removeClient(c->m_socket);
	}
	return nbytes;
}
