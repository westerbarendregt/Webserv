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

ssize_t	Server::receive(t_client *c) {
	if (!c)
		throw(serverError("getClient ", "client not registered"));

	char buf[1000];
	std::fill(buf, buf + sizeof(buf), 0);
	ssize_t nbytes = recv(c->m_socket, buf, sizeof(buf), 0);
	const char *to_append = buf;
	c->m_request_str.append(to_append);
	if (nbytes == -1)
		std::cout<<"recv: "<<strerror(errno)<<std::endl;
	return nbytes;
}
