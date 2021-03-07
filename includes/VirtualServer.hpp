#ifndef VIRTUAL_SERVER_HPP
# define VIRTUAL_SERVER_HPP

# include "Conf.hpp"
# include <sys/socket.h>
# include <netinet/in.h>

class	VirtualServer
{
	public:
		typedef	s_v_server_conf	t_v_server_conf;
		friend class Server;
		friend class Client;
		friend class ConfigParser;
		friend class VirtualContext;
		explicit VirtualServer(std::string host, t_v_server_conf conf);
		t_v_server_conf	*getVServerConf(std::string &host);
		void	init();
		void	close();
	private:
		void	setAddr();
		std::string				m_host; //debug
		int						m_socket;
		t_v_server_conf			m_configs;
		struct	sockaddr_in		m_sockaddr;
};

class	VirtualContext 
{
	public:
		friend class VirtualServer;
		friend class Server;
		friend class ConfigParser;
		friend class Client;
		typedef	 VirtualServer										t_v_server;
		typedef	 std::map<std::string, std::vector<VirtualServer> > 	t_v_server_host;
		VirtualContext();
		t_v_server	*getVirtualServer(std::string &host);
		t_v_server	*getVirtualServer(int socket);
	private:
		int	m_port;
		t_v_server_host m_v_server_host;
		t_v_server		*m_catch_all;
		void			setCatchAll();
};

#endif
