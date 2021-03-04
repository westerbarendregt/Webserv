#ifndef REQUEST_HANDLER
# define REQUEST_HANDLER

# include "Client.hpp"

class RequestHandler
{
	public:
		typedef	Client		t_client;
		void	handleMetadata(t_client &c);
		void	handleRequest(t_client &c);
	private:
};

#endif

