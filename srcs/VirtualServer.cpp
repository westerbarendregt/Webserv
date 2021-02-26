#include "VirtualServer.hpp"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "Error.hpp"
#include "utils.hpp"


VirtualServer::VirtualServer(t_v_server_conf conf) : m_configs(conf) {
	memset(&this->m_sockaddr, 0, sizeof(this->m_sockaddr));
}

void	VirtualServer::setAddr() {
	std::string	config_addr =  this->m_configs.m_directives["listen"];
	size_t	c = config_addr.find(':', 0);
	std::string port = c == std::string::npos ? config_addr : config_addr.substr(c + 1, config_addr.size() - c);
	if (port.empty() || port.find_first_not_of("0123456789", 0) != std::string::npos)
		throw serverError("server init", "invalid port");
	this->m_sockaddr.sin_family = AF_INET;
	this->m_sockaddr.sin_addr.s_addr = inet_addr(config_addr.substr(0, c).c_str());
	this->m_sockaddr.sin_port = hostToNetworkShort(ftAtoi(port.c_str()));
	memset(this->m_sockaddr.sin_zero, 0, sizeof(this->m_sockaddr.sin_zero));
}

void	VirtualServer::init() {
	int	enable = 1;

	this->setAddr();
	if ((this->m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw(serverError("socket", strerror(errno)));
	if (setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
		throw(serverError("setsockopt", strerror(errno)));
	if (bind(this->m_socket,  reinterpret_cast<struct sockaddr*>(&this->m_sockaddr), sizeof(this->m_sockaddr)) == -1)
	{
		//since v_server is mapped by ip:port, *:5000 and 127.0.0.1:5000 would throw already in use
		if (errno != EADDRINUSE)
			throw(serverError("bind", strerror(errno)));
	}
	else if (listen(this->m_socket, 128) == -1)
		throw(serverError("listen", strerror(errno)));
}

void	VirtualServer::close() {
	::close(this->m_socket);
}
