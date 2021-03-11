#include "ConfigParser.hpp"
#include "Server.hpp"

Server::Server(char const *path) {
	ConfigParser::parse(path, this->m_v_server_all);
}

std::vector<Server::t_v_server> *Server::getVirtualServer(int socket) {
	for (t_v_server_all::iterator v_server = this->m_v_server_all.begin(); v_server != this->m_v_server_all.end(); ++v_server) {
		if (v_server->second[0].m_socket == socket)
			return &v_server->second;
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
