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
#include "Logger.hpp"

VirtualServer::VirtualServer(t_v_server_conf conf)
: m_host(conf.m_directives["listen"]),
m_port(""),
m_socket(-1),
m_configs(conf),
m_sockaddr()
{
	std::fill(reinterpret_cast<unsigned char *>(&this->m_sockaddr), reinterpret_cast<unsigned char *>(&this->m_sockaddr) + sizeof(this->m_sockaddr), 0);
}

void	VirtualServer::setAddr() {
	std::string	config_addr =  this->m_configs.m_directives["listen"];
	size_t	c = config_addr.find(':', 0);
	this->m_port = c == std::string::npos ? config_addr : config_addr.substr(c + 1, config_addr.size() - c);
	if (this->m_port.empty() || this->m_port.find_first_not_of("0123456789", 0) != std::string::npos)
		throw serverError("server init", "invalid port");
	this->m_sockaddr.sin_family = AF_INET;
	this->m_sockaddr.sin_addr.s_addr = inet_addr(config_addr.substr(0, c).c_str());
	//Logger::Log()<<"ft::inet_ntoa()"<<ft::inet_ntoa(this->m_sockaddr.sin_addr)<<std::endl;
	this->m_sockaddr.sin_port = ft::hostToNetworkShort(ft::Atoi(this->m_port.c_str()));
	std::fill(this->m_sockaddr.sin_zero, this->m_sockaddr.sin_zero + sizeof(this->m_sockaddr.sin_zero), 0);
}

void	VirtualServer::init() {
	int	enable = 1;

	this->setAddr();
	if ((this->m_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw(serverError("socket", strerror(errno)));
	if (setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1)
		throw(serverError("setsockopt(SOL_REUSEADDR)", strerror(errno)));
	if (setsockopt(this->m_socket, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int)) == -1)
		throw(serverError("setsockopt(SO_REUSEPORT)", strerror(errno)));
}

VirtualServer::t_routes::iterator	VirtualServer::getLocation(t_request &request) {
	//compare against all route directives and find the longest one
	//since they are stored in map, they are already sorted by number of prefixes
	t_routes::iterator	it = this->m_configs.m_routes.begin();
	size_t	found, len, location_len = 0;
	size_t	const uri_len = request.m_path.length();
	while (it != this->m_configs.m_routes.end()) {
		location_len = it->first.length();
		len = location_len < uri_len ? location_len : uri_len;
		//len prevents location: dir/ uri: directory from matching since len characters has to match
		//so if uri > location, the whole uri has to match
		found = it->first.find(request.m_path.c_str(), 0, len);
		if (found == 0) {
			if (it->first == "/") 
				return it; //fallback
			if (location_len > uri_len && it->first[uri_len] == '/') 
				return it; // location: directory/ uri: directory
			if (uri_len == location_len)
				return it; //strictly equal
			if (location_len < uri_len)
				return it; // location: directory/ uri: directory/file
		}
		++it;
	}
	throw HTTPError("getLocation", "no location found", 500);
}
