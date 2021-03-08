#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include "VirtualServer.hpp"

struct	Request
{
		Request();
		int                     			m_method;
		std::string							m_path;
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
		bool								m_metadata_parsed;
		bool								m_done;
		int									m_error;
};

class	Client
{
	public:
		friend class Server;
		friend class RequestParser;
		friend class RequestHandler;

		explicit Client(VirtualServer *v_server, int socket);
		Client(int socket);
		Client();
		bool	fullMetaData();

		void		testingRequest(std::string str){
			m_request_str = str;
		}
		int									testGetError(){
			return m_request_data.m_error;
		}
		Request&							getRequest(){
			return m_request_data;						
		}
	private:

		std::string							m_request_str;
		VirtualServer						*m_v_server;
		int									m_socket;
		bool 								m_received;
		bool 								m_treated;
		struct	sockaddr_storage 			m_sockaddr;
		socklen_t							m_addrlen;
		Request								m_request_data;
		Response							m_reponse_data;
		std::string							m_response_str;
};

#endif
