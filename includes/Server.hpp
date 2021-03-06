#ifndef SERVER_HPP
# define SERVER_HPP

#include <vector>
#include <map>
#include <string>

#include "Conf.hpp"
#include "VirtualServer.hpp"
#include "Client.hpp"
#include "RequestHandler.hpp"

class	Server
{
	public:

		typedef	s_v_server_conf						t_v_server_conf;
		typedef	Client								t_client;
		typedef	std::string							t_ip_port;
		typedef	VirtualServer						t_v_server;
		typedef	RequestHandler						t_request_handler;
		typedef std::map<t_ip_port, std::vector<t_v_server> > t_v_server_map;
		//typedef	int									t_port;
		//typedef std::map<t_ip_port, std::vector<t_v_server> > t_v_server_port;
		//typedef std::map<t_port, t_server_port> t_v_server_map;
		typedef std::map<int, t_client> 	t_client_map;

		friend class RequestHandler;

		Server(char const *path);
		~Server();
		void	run();
		void	init();
		void	close();
		int		receive(t_client *c);
		void	respond(int client_socket);
		void	accept(int v_server_socket);
		void	addClient();
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
			t_request_handler	m_request_handler;
};

#endif
