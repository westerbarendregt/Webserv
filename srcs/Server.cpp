#include "Server.hpp"
#include "ConfigParser.hpp"

Server::Server(char const *path) {
	ConfigParser::parse(path, this->m_v_server_map);
}

VirtualServer *Server::getVirtualServer(int socket) {
	for (t_v_server_map::iterator it = this->m_v_server_map.begin(); it != this->m_v_server_map.end(); ++it) {
		if (it->second[0].m_socket == socket)
			return (&it->second[0]);
	}
	return (0);
}

Server::~Server() {}
