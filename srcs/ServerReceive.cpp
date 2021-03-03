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


void	Server::receive(int client_socket) {
	char buf[1000]; // can have buf elsewhere

	memset(buf, 0, sizeof(buf));
	t_client *c = this->getClient(client_socket);
	if (!c)
		throw(serverError("getClient ", "client not registered"));
	int	nbytes = recv(client_socket, buf, sizeof(buf), 0);
	const char *to_append = buf;
	c->m_request.append(to_append);
	if (nbytes <= 0){
		if (nbytes == 0)
			std::cout<<"closing connection "<<client_socket<<std::endl; //log
		else
			std::cout<<"recv: "<<strerror(errno)<<std::endl; //log
		::close(client_socket);
		FD_CLR(client_socket, &this->m_read_all);
		this->removeClient(client_socket);
	}
	else
	{
		if (c->fullHttpRequest()) //  this whole if statement would be integrated elsewhere
		{
			std::cout<<c->m_request<<std::endl;
			RequestParser::Parse(*c);
			RequestParser::Print(*c);
			FD_SET(c->m_socket, &this->m_write_all);
			std::cout<<"received full http request"<<std::endl;
		}
	}
}

//void	Server::read(t_client * c)
//{
//	char buf[1000]; // can have buf elsewhere
//
//	std::cout<<"reading... on "<<c->m_socket<<std::endl;
//	memset(buf, 0, sizeof(buf));
//	if (!c)
//		throw(serverError("getClient ", "client not registered"));
//	int	nbytes = recv(c->m_socket, buf, sizeof(buf), 0);
//	const char *to_append = buf;
//	c->m_request.append(to_append);
//	if (nbytes <= 0){
//		if (nbytes == 0)
//			std::cout<<"closing connection "<<c->m_socket<<std::endl; //log
//		else
//			std::cout<<"recv: "<<strerror(errno)<<std::endl; //log
//		::close(c->m_socket);
//		FD_CLR(c->m_socket, &this->m_all_fd);
//		this->removeClient(c->m_socket);
//	}
//	else {
//		std::cout<<"---current request---"<<std::endl;
//		std::cout<<c->m_request<<std::endl;
//	}
//}
