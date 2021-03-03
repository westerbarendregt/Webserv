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
					//if fullHttpRequest
					//	parse
					//	if client.m_request.done
					//		add to write_all
				}

			}
			else if (FD_ISSET(i, &this->m_write_fd)) {
				std::cout<<"found write connection fd: "<<i<<std::endl;
				this->handleRequest(i);
				//reset request
				//update response state
				
			}
			// while (1);
		}
	}
}
