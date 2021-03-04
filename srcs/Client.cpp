#include "Client.hpp"
#include "Server.hpp"
#include "Error.hpp"

Request::Request()
	: m_method(-1), m_path(""), m_protocol(-1), m_content_length(0), m_headers(0), m_if_body(false), m_body(""), m_done(false) 
{
	for (int i = 0; i < 18; ++i)
		m_headers.push_back("");
}

Response::Response()
	: m_method(-1), m_path(""), m_protocol(-1), m_content_length(0), m_headers(0), m_if_body(false), m_body(""), m_done(false) 
{
	for (int i = 0; i < 18; ++i)
		m_headers.push_back("");
}

Client::Client() 
	: m_request_str(""), m_v_server(0), m_socket(-1), m_received(false), m_treated(false), m_sockaddr(), m_addrlen(sizeof(m_sockaddr))
{
}

bool	Client::fullMetaData() {
	return (!this->m_request_str.empty()
			&& this->m_request_str.find("\r\n\r\n") != std::string::npos);
}


Client::Client(int socket) 
	: m_request_str(""), m_v_server(0), m_socket(socket), m_received(false), m_sockaddr(), m_addrlen(sizeof(m_sockaddr))
{

}

void	Server::removeClient(int client_socket) {
	if (this->m_client_map.erase(client_socket) != 1)
		throw serverError("removeClient: ", "trying to remove unexisting client");
}
