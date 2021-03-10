#include "VirtualServer.hpp"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include "Error.hpp"
#include "utils.hpp"
#include "Server.hpp"

VirtualServer::VirtualServer(t_v_server_conf conf)
: m_socket(-1),
m_configs(conf),
m_sockaddr()
{
	memset(&this->m_sockaddr, 0, sizeof(this->m_sockaddr));//fill
}

void	VirtualServer::setAddr() {
	std::string	config_addr =  this->m_configs.m_directives["listen"];
	size_t	c = config_addr.find(':', 0);
	std::string port = c == std::string::npos ? config_addr : config_addr.substr(c + 1, config_addr.size() - c);
	if (port.empty() || port.find_first_not_of("0123456789", 0) != std::string::npos)
		throw serverError("server init", "invalid port");
	this->m_sockaddr.sin_family = AF_INET;
	//std::cout<<"inet_() "<<config_addr.substr(0, c)<<std::endl;
	this->m_sockaddr.sin_addr.s_addr = inet_addr(config_addr.substr(0, c).c_str());
	//std::cout<<"inet_ntoa()"<<inet_ntoa(this->m_sockaddr.sin_addr)<<std::endl;
	this->m_sockaddr.sin_port = hostToNetworkShort(ftAtoi(port.c_str()));
	memset(this->m_sockaddr.sin_zero, 0, sizeof(this->m_sockaddr.sin_zero));//fill
}

void	VirtualServer::init() {
	int	enable = 1;

	this->setAddr();
	if ((this->m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw(serverError("socket", strerror(errno)));
	if (setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
		throw(serverError("setsockopt", strerror(errno)));
	if (setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) == -1)
		throw(serverError("setsockopt", strerror(errno)));
}

VirtualServer::t_v_server_conf	VirtualServer::*getVServerConf(std::string &) {
	return 0;
}
