#include "Server.hpp"
#include "ConfigParser.hpp"

Server::Server(char const *path) {
	ConfigParser::parse(path, this->m_v_servers);
}

Server::~Server() {}
