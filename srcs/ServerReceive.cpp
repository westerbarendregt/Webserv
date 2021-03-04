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


int	Server::receive(int client_socket) {
	char buf[1000]; // can have buf elsewhere

	memset(buf, 0, sizeof(buf));//fill
	t_client *c = this->getClient(client_socket);
	if (!c)
		throw(serverError("getClient ", "client not registered"));
	int	nbytes = recv(client_socket, buf, sizeof(buf), 0);
	const char *to_append = buf;
	c->m_request_str.append(to_append);
	if (nbytes <= 0){ // should we handle closing a connection in the ServerRun loop
		if (nbytes == 0)
			std::cout<<"closing connection "<<client_socket<<std::endl; //log
		else
			std::cout<<"recv: "<<strerror(errno)<<std::endl; //log
		::close(client_socket);
		FD_CLR(client_socket, &this->m_read_all);
		this->removeClient(client_socket);
	}
	return nbytes;
}
