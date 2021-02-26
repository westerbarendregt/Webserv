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

int	Server::read(int socket) {

	char buf[10000]; // can have buf elsewhere

	memset(buf, 0, sizeof(buf));
	//find client by socket 
	//t_client &c = this->getClient(socket);
	//and replace 
	int	nbytes = recv(socket, buf, sizeof(buf), 0);
	if (nbytes <= 0){
		if (nbytes == 0)
			std::cout<<"closing connection "<<socket<<std::endl; //log
		else
			std::cout<<"recv: "<<strerror(errno)<<std::endl; //log
		::close(socket);
		//FD_CLR(i, &master);
	}
	return (nbytes);
	//else {
	//	std::cout<<"---RECEIVED---"<<std::endl;
	//	//client.append(buf);
	//	std::cout<<buf<<std::endl;
	//	memset(buf, 0, sizeof(buf)); //this part in in request handler
	//}

}

int		Server::read(t_client const & c)
{

	char buf[10000]; // can have buf elsewhere

	memset(buf, 0, sizeof(buf));
	//find client by socket 
	//t_client &c = this->getClient(socket);
	//and replace 
	//
	std::string n_request = c.m_request;
	int	nbytes = recv(c.m_socket, buf, sizeof(buf), 0);
	//HTTP HEADER
	//
	//BODY
	if (nbytes > 0)
	{
		n_request.append(buf, buf + nbytes);
		std::cout<<"---READ---"<<std::endl;
		//client.append(buf);
		std::cout<<n_request<<std::endl;
		memset(buf, 0, sizeof(buf)); //this part in in request handler
	}
	else {
		if (nbytes == 0)
			std::cout<<"closing connection "<<c.m_socket<<std::endl; //log
		else
			std::cout<<"recv: "<<strerror(errno)<<std::endl; //log
		::close(c.m_socket);
		//FD_CLR(i, &master);
	}
	return (nbytes);
	//else {
	//	std::cout<<"---RECEIVED---"<<std::endl;
	//	//client.append(buf);
	//	std::cout<<buf<<std::endl;
	//	memset(buf, 0, sizeof(buf)); //this part in in request handler
	//}

}
