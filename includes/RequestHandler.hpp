#ifndef REQUEST_HANDLER
# define REQUEST_HANDLER

# include "VirtualServer.hpp"
# include "Cgi.hpp"
# include "Conf.hpp"
# include <sys/stat.h>
# include <limits.h>

class Client;

//do we need a class for Request Handler and a class Cgi, or simply a cgi section in the requesthandler class
class RequestHandler
{
	public:
		typedef	Client			t_client;
		typedef	VirtualServer	t_v_server;
		typedef	s_v_server_conf::t_directives t_directives;
		typedef	s_v_server_conf::t_routes	t_routes;
		void	handleMetadata(t_client &c);
		void	handleCgiMetadata(t_client &c);
		void	handleRequest(t_client &c);
	private:
			Cgi				m_cgi;
			struct	stat	m_statbuf;
};

#endif

