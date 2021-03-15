#ifndef VIRTUAL_SERVER_HPP
# define VIRTUAL_SERVER_HPP

# include "Conf.hpp"
# include <sys/socket.h>
# include <netinet/in.h>
# include "WebServer.hpp"

struct	Request;
class	VirtualServer
{
	public:
		typedef	s_v_server_conf	t_v_server_conf;
		typedef t_v_server_conf::t_directives t_directives;
		typedef t_v_server_conf::t_routes t_routes;
		typedef	Request			t_request;
		friend class Server;
		friend class Client;
		friend class ConfigParser;
		friend class RequestHandler;
		explicit VirtualServer(t_v_server_conf conf);
		void	init();
		void	close();
		t_routes::iterator	getLocation(t_request &r);
	private:
		void	setAddr();
		int						m_socket;
		t_v_server_conf			m_configs;
		struct	sockaddr_in		m_sockaddr;
};

#endif
