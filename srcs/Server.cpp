#include "ConfigParser.hpp"
#include "Server.hpp"
#include "Error.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

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

void	Server::closeClientConnection(t_client &c) {
	std::cout<<"closing connection "<<c.m_socket<<std::endl; //log
	::close(c.m_socket);
	if (FD_ISSET(c.m_socket, &this->m_read_all))
		FD_CLR(c.m_socket, &this->m_read_all);
	if (FD_ISSET(c.m_socket, &this->m_write_all))
		FD_CLR(c.m_socket, &this->m_write_all);
	this->m_request_handler.m_cgi.closeAll(c);
	if (c.m_cgi_running)
		this->m_request_handler.m_cgi.kill(c);
	if (this->m_client_map.erase(c.m_socket) != 1)
		throw serverError("removeClient: ", "trying to remove unexisting client");
}

Server::~Server() {}
