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
		bool								m_metadata_parsed;
		bool								m_done;
		bool								m_chunked;
		int									m_error;
};

struct	Response
{
		Response();
		int                     			m_method;
		char                        		m_path[1024];
		int                  	    		m_protocol;
		size_t								m_content_length;
		std::vector<std::string> 			m_headers;
		bool								m_if_body;
		std::string							m_body;
		int									m_error;
};

class	Client
{
	public:
		typedef	Request		t_request_data;
		typedef	Response	t_response_data;
		friend class Server;
		friend class RequestParser;
		friend class RequestHandler;

		explicit Client(VirtualServer *v_server, int socket);
		Client(int socket);
		Client();
		bool	fullMetaData();
	private:
		std::string							m_request_str;
		std::string							m_response_str;
		t_request_data 						m_request_data;
		t_response_data						m_response_data;
		int									m_socket;
		struct	sockaddr_storage 			m_sockaddr;
		socklen_t							m_addrlen;
};

#endif
