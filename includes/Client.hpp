#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include "VirtualServer.hpp"
# include "Conf.hpp"

class	Client;
struct	Request
{
		Request();
		Request(Request const & src);
		Request &operator=(Request const & rhs);
		Client								*m_owner;
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
		bool								m_cgi;
		bool								m_autoindex;
		int									m_error;
		size_t								m_start;
		s_v_server_conf::t_routes::iterator m_location;
		std::string							m_query_string;
		std::string							m_path_info;
		std::string							m_real_path;
		std::string							m_file;
		size_t								m_cgi_write;
};

struct	Response
{
		Response();
		std::string	m_content_type;
};

class	Client
{
	public:
		typedef	Request							t_request_data;
		typedef	Response						t_response_data;
		typedef VirtualServer					t_v_server;
		typedef VirtualServer::t_v_server_conf	t_v_server_conf;
		typedef	std::vector<t_v_server>			t_v_server_blocks;
		friend class Server;
		friend class RequestParser;
		friend class RequestHandler;
		friend class Cgi;

		Client();
		Client(Client const & src);
		Client 	&operator=(Client const & rhs);
		bool	fullMetaData();
		void	updateServerConf();

		void		testingRequest(std::string str){
			m_request_str = str;
		}
		int									testGetError(){
			return m_request_data.m_error;
		}
		Request&				 			getRequest(){
			return m_request_data;						
		}
	private:
		std::string							m_request_str;
		std::string							m_response_str;
		t_request_data 						m_request_data;
		t_response_data						m_response_data;
		t_v_server							*m_v_server;
		t_v_server_blocks					*m_v_server_blocks;
		int									m_socket;
		struct	sockaddr_storage 			m_sockaddr;
		socklen_t							m_addrlen;
		pid_t								m_cgi_pid;
		bool								m_cgi_running;
		size_t								m_cgi_write;
		int									m_cgi_read_pipe[2];
		int									m_cgi_write_pipe[2];
		std::string							m_cgi_out_buf;
};

#endif
