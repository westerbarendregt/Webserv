#include "Server.hpp"
#include "Error.hpp"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

Server::t_client *Server::accept(int v_server_socket) {
	t_client	c;

	if ((c.m_socket = ::accept(v_server_socket, reinterpret_cast<struct sockaddr *>(&c.m_sockaddr), &c.m_addrlen)) == -1) {
		std::cout<<"accept: "<<strerror(errno)<<std::endl;
		return 0; //can throw WOULDBLOCK
	}
	if (fcntl(c.m_socket, F_SETFL, O_NONBLOCK)  == -1)
		throw(serverError("fcntl: ", strerror(errno)));
	std::cout<<"adding client socket "<<c.m_socket<<std::endl;
	this->m_client_map[c.m_socket] = c;
	FD_SET(c.m_socket, &this->m_read_all);
	if (c.m_socket > this->m_range_fd)
		this->m_range_fd = c.m_socket;
	return &this->m_client_map[c.m_socket];
}
