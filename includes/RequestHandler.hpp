#ifndef REQUEST_HANDLER
# define REQUEST_HANDLER

# include "VirtualServer.hpp"

class Client;

class RequestHandler
{
	public:
		typedef	Client			t_client;
		typedef	VirtualServer	t_v_server;
		typedef	VirtualContext	t_v_context;
		void	handleMetadata(t_client &c);
		void	handleRequest(t_client &c);
	private:
};

#endif

