#include "Server.hpp"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "Error.hpp"


void	Server::init(){
	size_t	server_block, n_server;

	FD_ZERO(&this->m_write_all);
	FD_ZERO(&this->m_read_all);
	FD_ZERO(&this->m_write_fd);
	FD_ZERO(&this->m_read_fd);
	this->m_range_fd = 0;
	for (t_v_server_map::iterator it = this->m_v_server_map.begin(); 
			it != this->m_v_server_map.end(); ++it) {
		it->second[0].init(); // start default virtual server
		this->connectVirtualServer(it->second[0]);
		server_block = 0;
		n_server = it->second.size();
		while (++server_block < n_server) {
			it->second[server_block].m_socket = it->second[0].m_socket; // assign default virtual server's socket to remaining blocks for same port.
			it->second[server_block].m_sockaddr = it->second[0].m_sockaddr;
		}
		std::cout<<"v_server port "<<it->first<<" listens on socket "<<it->second[0].m_socket<<std::endl;
	}
}

void	Server::connectVirtualServer(VirtualServer &v_server) {
	if (bind(v_server.m_socket,  reinterpret_cast<struct sockaddr *>(&v_server.m_sockaddr), sizeof(v_server.m_sockaddr)) == -1)
	{
		//since v_server is mapped by ip:port, *:5000 and 127.0.0.1:5000 would throw already in use
		//so we just try to link it with an already existing socket
		//
		if (errno == EADDRINUSE) {
			VirtualServer	*other;
			std::cout<<"already in use"<<std::endl;
			if ((other = this->getVirtualServer(v_server.m_sockaddr.sin_port)))
			{
				std::cout<<"connecting with server "<< other->m_socket<<std::endl;
				v_server.m_socket = other->m_socket;
				return ;
			}
		}
		throw(serverError("bind", strerror(errno)));
	}
	else if (listen(v_server.m_socket, 128) == -1)
		throw(serverError("listen", strerror(errno)));
	if (fcntl(v_server.m_socket, F_SETFL, O_NONBLOCK) == -1)
		throw(serverError("fcntl: ", strerror(errno)));
	FD_SET(v_server.m_socket, &this->m_read_all);
	if (v_server.m_socket > this->m_range_fd)
		this->m_range_fd = v_server.m_socket;
}

void	Server::close() {
	for (t_v_server_map::iterator it = this->m_v_server_map.begin(); 
			it != this->m_v_server_map.end(); ++it)
		it->second[0].close();
	FD_ZERO(&this->m_write_all);
	FD_ZERO(&this->m_read_all);
	FD_ZERO(&this->m_write_fd);
	FD_ZERO(&this->m_read_fd);
}
