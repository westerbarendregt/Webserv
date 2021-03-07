#include "ConfigParser.hpp"
#include "Server.hpp"

Server::Server(char const *path) {
	ConfigParser::parse(path, this->m_v_server_all);
}

Server::t_v_context *Server::getVirtualContext(int port) {
	t_v_server_all::iterator	found;

	if ((found = this->m_v_server_all.find(port)) != this->m_v_server_all.end()) {
		return &found->second;
	}
	return 0;
}

Server::t_v_server	*Server::getVirtualServer(int socket) {
	t_v_server	*found = 0;
	for (t_v_server_all::iterator port = this->m_v_server_all.begin(); port != this->m_v_server_all.end(); ++port) {
		if ((found = port->second.getVirtualServer(socket)))
			return found;
	}
	return 0;
}

Server::t_client	*Server::getClient(int client_socket) {
	t_client_map::iterator found = this->m_client_map.find(client_socket);
	if (found != this->m_client_map.end())
		return (&(found->second));
	return (0);
}

Server::~Server() {}
