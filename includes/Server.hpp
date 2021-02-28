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

		Server(char const *path);
		~Server();
		void	run();
		void	init();
		void	close();
		int		read(int socket);
		int		read(t_client const & c);
		void	accept();
		void	addClient();
		void	handleRequest(int socket);
		void	connectVirtualServer(t_v_server &v_server);
		t_v_server	*getVirtualServer(int v_server_socket);
		t_v_server	*getVirtualServer(unsigned short port);
	private:
			t_v_server_map	m_v_server_map;
};

#endif
