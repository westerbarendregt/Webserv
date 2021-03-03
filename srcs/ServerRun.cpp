#include "Server.hpp"
#include "Error.hpp"
#include "Client.hpp"
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
	t_v_server	*v_server;
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
				v_server = getVirtualServer(i); //find corresponding v_server
				std::cout<<"found read connection fd: "<<i<<std::endl;
				if (v_server)
					c = this->accept(i);
				else
				{
					this->receive(i);
					// if this->receive(i) > 0
					// {
					// 		if (!c->m_request.parsed) { //metadata parsed
					// 			if (!fullMetadata()) // fullHttpRequest with better name
					// 				continue ;
					// 			RequestParser::Parse(*c);
					// 			handleMetadata(*c); 
					// 			//perform checks, lstat, detects chunk fields.. and update m_request_parse and c->m_request done if no body to expect
					// 			//
					// 		}
					// 		if (c->m_request.done)
					// 			handleRequest(); // either get resource or execute cgi, both populating the response and adding client_socket to write_all
					// 		else
					// 			handleBody(); // adds to body, updates necessary information, compares lenght of body with Content-Length header field
					// 						// if chunk, appends to body and searches for end chunk
					// 						//updates c->m_request.done when detects end of body
					// }
					// if we detect any errors that requires responding with an error page,
					// we can call something that sets the response to the appropriate status page and add the client's socket to write_all
					// it would also mark the request as done
				}

			}
			else if (FD_ISSET(i, &this->m_write_fd)) {
				std::cout<<"found write connection fd: "<<i<<std::endl;
				this->handleRequest(i); // to be replaced by this->Respond
				//respond can close connection if the response is an error
				
			}
		}
	}
}
