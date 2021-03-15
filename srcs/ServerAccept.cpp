#include "Server.hpp"
#include "Error.hpp"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

int	Server::accept(int socket) {
	t_v_server_blocks *v;
	t_client c;

	if (!(v = this->getVirtualServer(socket)))
	{
		std::cout<<"no listeners with socket "<<socket<<std::endl;
		return 1; //not a new connection
	}
	c.m_v_server_blocks = v;
	std::cout<<"found virtual_server with socket"<<(*v)[0].m_socket<<std::endl;
	if ((c.m_socket= ::accept((*v)[0].m_socket, reinterpret_cast<struct sockaddr *>(&c.m_sockaddr), &c.m_addrlen)) == -1) {
		std::cout<<"accept: "<<strerror(errno)<<std::endl;
		return 1; //can throw WOULDBLOCK
	}
	if (fcntl(c.m_socket, F_SETFL, O_NONBLOCK)  == -1)
		throw(serverError("fcntl: ", strerror(errno)));
	std::cout<<"adding client socket "<<c.m_socket<<std::endl;
	this->m_client_map[c.m_socket] = c;
	FD_SET(c.m_socket, &this->m_read_all);
	if (c.m_socket > this->m_range_fd)
		this->m_range_fd = c.m_socket;
	return 0;
}
