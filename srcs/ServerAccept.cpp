#include "Server.hpp"
#include "Error.hpp"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "Logger.hpp"

int	Server::accept(int socket) {
	const int MAX_CLIENTS = 1000;
	t_v_server_blocks *v;
	t_client c;

	if (!(v = this->getVirtualServer(socket)))
	{
		return 1; //not a new connection
	}
	c.m_v_server_blocks = v;
	Logger::Log()<<"found virtual_server with socket"<<(*v)[0].m_socket<<std::endl;
	if ((c.m_socket= ::accept((*v)[0].m_socket, reinterpret_cast<struct sockaddr *>(&c.m_sockaddr), &c.m_addrlen)) == -1) {
		Logger::Log()<<"accept: "<<strerror(errno)<<std::endl;
		return 1; //can throw WOULDBLOCK
	}
	if (fcntl(c.m_socket, F_SETFL, O_NONBLOCK)  == -1)
		throw(serverError("fcntl: ", strerror(errno)));
	Logger::Log()<<"adding client socket "<<c.m_socket<<std::endl;
	this->m_client_all[c.m_socket] = c;
	if (this->m_client_all.size() > MAX_CLIENTS) {
		c.m_request_data.m_status_code = 503;
		this->m_request_handler.SetRetryAfter();
		this->m_request_handler.handleRequest(c);
		this->respond(c);
		return 0;
	}
	FD_SET(c.m_socket, &this->m_read_all);
	if (c.m_socket > this->m_range_fd)
		this->m_range_fd = c.m_socket;
	return 0;
}
