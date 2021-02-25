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


#define PORT "5000"

void	Server::run(){
	
	fd_set master, read_fs, write_fs;
	//fd_set master, read_fs;
	int range;
	int	s_listen, s_new;
	struct sockaddr_storage clientaddr;
	struct sockaddr_storage serveraddr;
	socklen_t	addrlen;
	int			enable =1;

	/*test getaddrinfo */
	struct addrinfo hints, *returned, *current;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family= AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	//get server info
	int gai;
	if ((gai = getaddrinfo(NULL, PORT, &hints, &returned)) != 0)
		throw(serverError("getaddrinfo: ", gai_strerror(gai)));

	//iter through returned addrinfo structures
	for (current = returned; current != NULL; current=current->ai_next) {

		s_listen = socket(AF_INET, SOCK_STREAM, 0);
		if (s_listen < 0) {
			std::cout<<"listen<0 skip"<<std::endl;
			continue ;
			//throw(serverError("listen: ", strerror(errno)));
		}

		//SOL_SOCKET api level
		if (setsockopt(s_listen, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
			//throw(serverError("setsockopt: ", strerror(errno)));
			std::cout<<"setopt<0 skip"<<std::endl;
			continue ;
		}

		std::cout<<"bind with current->ai_addr->sa_family  "<<current->ai_addr->sa_family<<std::endl;
		if (bind(s_listen, current->ai_addr, current->ai_addrlen) < 0 ) {
			//throw(serverError("setsockopt: ", strerror(errno)));
			std::cout<<"bind<0 skip"<<std::endl;
			continue ;
		}
		break ; //valid addr
	}
	if (current == NULL){ // went through the whole thing without finding a valid addr
		throw(serverError("getserver info, no valid addr returned: ", strerror(errno)));
	}
	freeaddrinfo(returned);

	//set listener  to non_blocking
	if (listen(s_listen, 128) == -1)
		throw(serverError("listen: ", strerror(errno)));

	if (fcntl(s_listen, F_SETFL, O_NONBLOCK) == -1)
		throw(serverError("fcntl: ", strerror(errno)));
	FD_SET(s_listen, &master);
	range = s_listen; //fd_max
	struct timeval tv;
	char buf[1000];
	memset(buf, 0, sizeof(buf));
	for (;;) {
		read_fs = master;
		write_fs = master;
		tv.tv_sec = 1; //on linux, tv is modified so need to reset for every call
		tv.tv_usec = 500000;
		if (select(range + 1, &read_fs, &write_fs, NULL, &tv) == -1)
			throw(serverError("select: ", strerror(errno)));
		std::cout<<"listening..."<<std::endl;
		for (int i =0; i <= range;i++) {
			if (FD_ISSET(i, &read_fs)) {
				std::cout<<"found read connection fd: "<<i<<std::endl;
				if (i == s_listen) { //isInListeners
					std::cout<<"new connection" <<std::endl;
					addrlen = sizeof(clientaddr);
					s_new = ::accept(s_listen, reinterpret_cast<struct sockaddr *>(&clientaddr), &addrlen);
					if (fcntl(s_new, F_SETFL, O_NONBLOCK)  == -1)
						throw(serverError("fcntl: ", strerror(errno)));
					if (s_new == -1) {
						std::cout<<"accept: "<<strerror(errno)<<std::endl;
					}
					else {
						//create
						std::cout<<"created client socket " <<s_new<<std::endl;
						FD_SET(s_new, &master);
						if (s_new > range) {
							range = s_new;
							/*print log here*/
						}
					}
				}
				else {
					//find client by socket, example with only one
					t_client c(i);

					//and pass it to this read
					if (this->read(c) <= 0)
						FD_CLR(i, &master);
					//we have clients
					//findClient(i)
					//
					//append().buf
				//	int nbytes=recv(i, buf, sizeof buf, 0);
				//	if (nbytes <= 0){
				//		if (nbytes == 0)
				//			std::cout<<"closing connection "<<i<<std::endl;
				//		else
				//			std::cout<<"recv: "<<strerror(errno)<<std::endl;
				//		close(i);
				//		FD_CLR(i, &master);
				//	}
				//	else {
				//		std::cout<<"---RECEIVED---"<<std::endl;
				//		//client.append(buf);
				//		std::cout<<buf<<std::endl;
				//		memset(buf, 0, sizeof(buf));
				//	}
				}
			}
			else if (FD_ISSET(i, &write_fs))
			{
				//get client(i)
				//requesthandler(client)
				//connection ready to write, call request handler
				std::cout<<"found write connection fd: "<<i<<std::endl;
				std::cout<<"---treating---"<<std::endl;
				//handle request(client)
				write(i, "OK", 2);
			}
		}
	}


	FD_ZERO(&master);
	FD_ZERO(&read_fs);

	//AF_INET ipv4


	close(s_listen);

	(void) master; (void)read_fs;
	(void)range;
	(void)s_listen; (void)s_new;
	(void)clientaddr;
	(void)serveraddr;
	(void)addrlen;
}
