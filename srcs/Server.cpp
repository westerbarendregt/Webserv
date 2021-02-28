#include "Server.hpp"
#include "ConfigParser.hpp"

Server::Server(char const *path) {
	ConfigParser::parse(path, this->m_v_server_map);
	FD_ZERO(&this->m_all_fd);
	m_range_fd = 0;
}

VirtualServer *Server::getVirtualServer(int v_server_socket) {
	for (t_v_server_map::iterator it = this->m_v_server_map.begin(); it != this->m_v_server_map.end(); ++it) {
		if (it->second[0].m_socket == v_server_socket)
			return (&it->second[0]);
	}
	return (0);
}

Server::t_v_server	*Server::getVirtualServer(unsigned short port) {
	for (t_v_server_map::iterator it = this->m_v_server_map.begin(); it != this->m_v_server_map.end(); ++it) {
		if (it->second[0].m_sockaddr.sin_port == port)
			return (&it->second[0]);
	}
	return (0);
}

Server::~Server() {}
