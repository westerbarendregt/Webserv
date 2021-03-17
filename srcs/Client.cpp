#include "Client.hpp"
#include "Server.hpp"
#include "Error.hpp"
#include "WebServer.hpp"
#include <unistd.h>

Request::Request()
	: m_owner(0),
	m_method(-1),
	m_path(""),
	m_protocol(-1),
	m_content_length(0),
	m_headers(18, ""),
	m_if_body(false),
	m_body(""),
	m_metadata_parsed(false),
	m_done(false),
	m_chunked(false),
	m_cgi(0),
	m_autoindex(0),
	m_error(0),
	m_start(0),
	m_location(),
	m_query_string(""),
	m_path_info(""),
	m_real_path(""),
	m_file("")
{
}

Request::Request(Request const & src) 
	:m_method(src.m_method),
	 m_path(src.m_path),
	 m_protocol(src.m_protocol),
	 m_content_length(src.m_content_length),
	m_headers(src.m_headers.begin(), src.m_headers.end()),
	 m_if_body(src.m_if_body),
	 m_body(src.m_body),
	 m_metadata_parsed(src.m_metadata_parsed),
	 m_done(src.m_done),
	 m_chunked(src.m_chunked),
	 m_cgi(src.m_cgi),
	 m_autoindex(src.m_autoindex),
	 m_error(src.m_error),
	 m_start(src.m_start),
	 m_location(src.m_location),
	 m_query_string(src.m_query_string),
	 m_path_info(src.m_path_info),
	 m_real_path(src.m_real_path),
	 m_file(src.m_file),
	 m_cgi_write(src.m_cgi_write)
{
}

Request &Request::operator=(Request const & rhs) {
	 this->m_method         = rhs.m_method;
	 this->m_path           = rhs.m_path;
	 this->m_protocol       = rhs.m_protocol;
	 this->m_content_length = rhs.m_content_length;
	 this->m_headers.assign(rhs.m_headers.begin(), rhs.m_headers.end());
	 this->m_if_body        = rhs.m_if_body;
	 this->m_body           = rhs.m_body;
	 this->m_metadata_parsed= rhs.m_metadata_parsed;
	 this->m_done           = rhs.m_done;
	 this->m_chunked        = rhs.m_chunked;
	 this->m_cgi            = rhs.m_cgi;
	 this->m_autoindex      = rhs.m_autoindex;
	 this->m_error          = rhs.m_error;
	 this->m_start          = rhs.m_start;
	 this->m_location       = rhs.m_location;
	 this->m_query_string   = rhs.m_query_string;
	 this->m_path_info      = rhs.m_path_info;
	 this->m_real_path      = rhs.m_real_path;
	 this->m_file           = rhs.m_file;
	 this->m_cgi_write      = rhs.m_cgi_write;
	return *this;
}

Response::Response()
	: m_content_type()
{
}

Client::Client() 
	: m_request_str(""),
	m_response_str(""),
	m_request_data(),
	m_response_data(),
	m_v_server(0),
	m_socket(-1),
	m_sockaddr(),
	m_addrlen(sizeof(m_sockaddr)),
	m_cgi_pid(-1),
	m_cgi_running(0),
	m_cgi_write(0)
{
	this->m_request_data.m_owner = this;
}

bool	Client::fullMetaData() {
	return (!this->m_request_str.empty()
			&& this->m_request_str.find("\r\n\r\n") != std::string::npos);
}

Client::Client(Client const & src)
	: m_request_str(src.m_request_str),
	m_response_str(src.m_response_str),
	m_request_data(src.m_request_data),
	m_response_data(src.m_response_data),
	m_v_server(src.m_v_server),
	m_v_server_blocks(src.m_v_server_blocks),
	m_socket(src.m_socket),
	m_sockaddr(src.m_sockaddr),
	m_addrlen(src.m_addrlen),
	m_cgi_pid(src.m_cgi_pid),
	m_cgi_running(src.m_cgi_running),
	m_cgi_write(src.m_cgi_write)
{
	this->m_request_data.m_owner = this;
}

Client &Client::operator=(Client const & rhs) {
	this->m_request_str = rhs.m_request_str;
	this->m_response_str = rhs.m_response_str;
	this->m_request_data = rhs.m_request_data;
	this->m_v_server = rhs.m_v_server;
	this->m_v_server_blocks = rhs.m_v_server_blocks;
	this->m_socket = rhs.m_socket;
	this->m_sockaddr = rhs.m_sockaddr;
	this->m_addrlen = rhs.m_addrlen;
	this->m_cgi_pid = rhs.m_cgi_pid;
	this->m_cgi_running = rhs.m_cgi_running;
	this->m_cgi_write= rhs.m_cgi_write;
	this->m_request_data.m_owner = this;
	return *this;
}


void	Client::updateServerConf()
{
	std::string host2 = this->m_request_data.m_headers[HOST];
	if (host2.empty()) {
		throw HTTPError("updateServerConf", "empty host header." , 400);
	}
	host2.resize(host2.size() - 1); // remove when problem fixed in RequestParser
	for (size_t i = 0; i < (*(this->m_v_server_blocks)).size(); ++i) {
		if ((*(this->m_v_server_blocks))[i].m_configs.m_directives["server_name"] == host2) {
			this->m_v_server = &(*(this->m_v_server_blocks))[i];//select server by server_name
			return ;
		}
	}
	this->m_v_server = &((*(this->m_v_server_blocks))[0]);//if not found, return the first added,default one
}
