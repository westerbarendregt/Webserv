#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include "VirtualServer.hpp"

struct	Request
{
		Request();
		int                     			m_method;
		char                        		m_path[1024];
		int                  	    		m_protocol;
		size_t								m_content_length;
		std::vector<std::string> 			m_headers;
		bool								m_if_body;
		std::string							m_body;
		bool								m_done;
};

class	Client
{
	public:
		friend class Server;
		friend class RequestParser;

		explicit Client(VirtualServer *v_server, int socket);
		Client(int socket);
		Client();
		bool	fullHttpRequest();

	private:

		std::string							m_request;
		VirtualServer						*m_v_server;
		int									m_socket;
		bool 								m_received;
		bool 								m_treated;
		struct	sockaddr_storage 			m_sockaddr;
		socklen_t							m_addrlen;
		Request								m_data;
};

#endif
