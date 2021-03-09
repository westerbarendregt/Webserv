#include "Client.hpp"
#include "Server.hpp"
#include "Error.hpp"

Request::Request()
	: m_method(-1),
	m_path(""),
	m_protocol(-1),
	m_content_length(0),
	m_headers(0),
	m_if_body(false),
	m_body(""),
	m_metadata_parsed(false),
	m_done(false),
	m_chunked(false),
	m_error(0),
	m_start(0)
{
	for (int i = 0; i < 18; ++i)
		m_headers.push_back("");
}

Response::Response()
	: m_method(-1),
	m_path(""),
	m_protocol(-1),
	m_content_length(0),
	m_headers(0),
	m_if_body(false),
	m_body(""),
	m_error(0)
{
	for (int i = 0; i < 18; ++i)
		m_headers.push_back("");
}

Client::Client() 
	: m_request_str(""),
	m_response_str(""),
	m_request_data(),
	m_response_data(),
	m_v_context(),
	m_v_server(0),
	m_socket(-1),
	m_sockaddr(),
	m_addrlen(sizeof(m_sockaddr))
{
}

Client::Client(t_v_context &v_context) 
	: m_request_str(""),
	m_response_str(""),
	m_request_data(),
	m_response_data(),
	m_v_context(&v_context),
	m_v_server(0),
	m_socket(-1),
	m_sockaddr(),
	m_addrlen(sizeof(m_sockaddr))
{
}


Client::Client(Client const & src)
	: m_request_str(src.m_request_str),
	m_response_str(src.m_response_str),
	m_request_data(src.m_request_data),
	m_response_data(src.m_response_data),
	m_v_context(src.m_v_context),
	m_socket(src.m_socket),
	m_sockaddr(src.m_sockaddr),
	m_addrlen(src.m_addrlen)
{

}


bool	Client::fullMetaData() {
	return (!this->m_request_str.empty()
			&& this->m_request_str.find("\r\n\r\n") != std::string::npos);
}

void	Server::removeClient(int client_socket) {
	if (this->m_client_map.erase(client_socket) != 1)
		throw serverError("removeClient: ", "trying to remove unexisting client");
}

// bool	Client::fullMetaData()
// {
// 	if (m_request_str.find("\n\n") != std::string::npos)
// 		return true;
// 	return false;
// }
