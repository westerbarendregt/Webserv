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
		friend class RequestHandler;
		friend class Cgi;
		explicit VirtualServer(t_v_server_conf conf);
		t_v_server_conf	*getVServerConf(std::string &host);
		void	init();
		void	close();
	private:
		void	setAddr();
		std::string				&m_host;
		std::string				m_port;
		int						m_socket;
		t_v_server_conf			m_configs;
		struct	sockaddr_in		m_sockaddr;
};

#endif
