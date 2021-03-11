#include "Server.hpp"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "Error.hpp"


void	Server::init(){
	FD_ZERO(&this->m_write_all);
	FD_ZERO(&this->m_read_all);
	FD_ZERO(&this->m_write_fd);
	FD_ZERO(&this->m_read_fd);
	this->m_range_fd = 0;
	for (t_v_server_all::iterator v_server = this->m_v_server_all.begin(); 
			v_server != this->m_v_server_all.end(); ++v_server) {
		v_server->second[0].init();
		this->connectVirtualServer(v_server->second[0]);
		size_t	i = 0;
		while (++i < v_server->second.size()) {
			v_server->second[i].m_socket = v_server->second[0].m_socket;
		}
	}
}

void	Server::connectVirtualServer(t_v_server &v_server) {
	if (bind(v_server.m_socket,  reinterpret_cast<struct sockaddr *>(&v_server.m_sockaddr), sizeof(v_server.m_sockaddr)) == -1)
		throw(serverError("bind", strerror(errno)));
	else if (listen(v_server.m_socket, 128) == -1)
		throw(serverError("listen", strerror(errno)));
	if (fcntl(v_server.m_socket, F_SETFL, O_NONBLOCK) == -1)
		throw(serverError("fcntl: ", strerror(errno)));
	FD_SET(v_server.m_socket, &this->m_read_all);
	if (v_server.m_socket > this->m_range_fd)
		this->m_range_fd = v_server.m_socket;
	std::cout<<v_server.m_configs.m_directives["listen"]<<" listens on socket "<<v_server.m_socket<<std::endl;
}

void	Server::close() {
	for (t_v_server_all::iterator v_server = this->m_v_server_all.begin(); 
			v_server != this->m_v_server_all.end(); ++v_server)
		::close(v_server->second[0].m_socket);
	FD_ZERO(&this->m_write_all);
	FD_ZERO(&this->m_read_all);
	FD_ZERO(&this->m_write_fd);
	FD_ZERO(&this->m_read_fd);
}
