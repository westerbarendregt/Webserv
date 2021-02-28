#include "Client.hpp"
#include "Server.hpp"
#include "Error.hpp"

Client::Client() 
	: m_v_server(0), m_socket(-1), m_request(""), m_received(false), m_treated(false), m_sockaddr(), m_addrlen(sizeof(m_sockaddr))
{

}

bool	Client::fullHttpRequest() {
	return (!this->m_request.empty()
			&& this->m_request.find("\r\n\r\n") != std::string::npos);
}


Client::Client(int socket) 
	: m_v_server(0), m_socket(socket), m_request(""), m_received(false), m_sockaddr(), m_addrlen(sizeof(m_sockaddr))
{

}

void	Server::removeClient(int client_socket) {
	if (this->m_client_map.erase(client_socket) != 1)
		throw serverError("removeClient: ", "trying to remove unexisting client");
}
