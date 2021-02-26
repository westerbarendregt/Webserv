#include "Server.hpp"

void	Server::init(){
	size_t	server_block, n_server;
	for (t_v_server_map::iterator it = this->m_v_server_map.begin(); 
			it != this->m_v_server_map.end(); ++it) {
		it->second[0].init(); // start default virtual server
		server_block = 0;
		n_server = it->second.size();
		while (++server_block < n_server) {
			it->second[server_block].m_socket = it->second[0].m_socket; // assign default virtual server's socket to remaining blocks for same port.
			it->second[server_block].m_sockaddr = it->second[0].m_sockaddr;
		}
	}
}

void	Server::close() {
	for (t_v_server_map::iterator it = this->m_v_server_map.begin(); 
			it != this->m_v_server_map.end(); ++it)
		it->second[0].close();

}
