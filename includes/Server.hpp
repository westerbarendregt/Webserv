#ifndef SERVER_HPP
# define SERVER_HPP

#include <vector>
#include <map>
#include <string>

#include "Conf.hpp"
#include "VirtualServer.hpp"
#include "Client.hpp"

class	Server
{
	public:

		typedef	s_v_server_conf						t_v_server_conf;
		typedef	Client								t_client;
		typedef	std::string							t_ip_port;
		typedef	VirtualServer						t_v_server;
		typedef std::map<t_ip_port, std::vector<t_v_server> > t_v_server_map;
		typedef std::map<int, t_client> 	t_client_map;

		Server(char const *path);
		~Server();
		void	run();
		void	init();
		void	close();
		void	receive(int socket);
		void	respond(t_client &c);
		t_client *accept(int v_server_socket); // looks for listeners based on the socket, if found accepts it, if not just returns
		void	addClient();
		void	handleRequest(int socket);
		void	connectVirtualServer(t_v_server &v_server);
		void	removeClient(int client_socket);
		t_v_server	*getVirtualServer(int v_server_socket);
		t_v_server	*getVirtualServer(unsigned short port);
		t_client	*getClient(int client_socket);
		fd_set		m_read_all;
		fd_set		m_write_all;
		fd_set		m_read_fd;
		fd_set		m_write_fd;
		int			m_range_fd;
	private:
			t_v_server_map	m_v_server_map;
			t_client_map	m_client_map;
};

#endif
