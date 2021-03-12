#include "Server.hpp"
#include "Error.hpp"
#include "Client.hpp"
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




void	Server::run(){
	
	struct timeval tv;
	t_client 	*c;

	for (;;) {//run
		this->m_read_fd = this->m_read_all;
		this->m_write_fd = this->m_write_all;
		tv.tv_sec = 1; //on linux, tv is modified so need to reset for every call
		tv.tv_usec = 500000;
		if (select(this->m_range_fd + 1, &this->m_read_fd, &this->m_write_fd, NULL, &tv) == -1)
			throw(serverError("select: ", strerror(errno)));
		std::cout<<"listening..."<<std::endl;
		for (int i =0; i <= this->m_range_fd ; ++i){
			if (FD_ISSET(i, &this->m_read_fd)) {
				std::cout<<"found read connection fd: "<<i<<std::endl;
				if (this->accept(i) == SUCCESS)
					continue ;
				c = getClient(i);
				if (this->receive(c) > 0) {
				 	if (!c->m_request_data.m_metadata_parsed) {
				 		if (!c->fullMetaData())
				 			continue ;
						std::cout<<"received full metadata"<<std::endl;
						// if (parse != error)
						// 	this->handleMetadata(*c); 
						// 	if error, flag the request as done, and as erroneous, so handle request can generate a error page.
						std::cout<<"======"<<std::endl;
						std::cout<<c->m_request_str<<std::endl;
						std::cout<<"======"<<std::endl;
				 		RequestParser::Parse(*c);
						c->m_request_data.m_metadata_parsed = true;
						RequestParser::Print(*c);
				 		this->m_request_handler.handleMetadata(*c); 
				 	}
				 	if (c->m_request_data.m_done)
					{
				 		this->m_request_handler.handleRequest(*c);
						FD_SET(c->m_socket, &this->m_write_all);
						// reset client struct and request.str()
						c->m_request_str.clear();
						//maybe at this point we want to remove it from the read_all
						//set, to not get a read from select while we haven't
						//sent the full response
					}
				 	else
				 		RequestParser::HandleBody(*c);
				 }
			}
			else if (FD_ISSET(i, &this->m_write_fd)) {
				std::cout<<"found write connection fd: "<<i<<std::endl;
				this->respond(i);
				//can close connection if the response is an error
				//resetResponse
			}
	}
}
}
