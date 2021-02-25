#include "Client.hpp"

Client::Client(VirtualServer *v_server, int socket) 
	: m_v_server(v_server), m_socket(socket), m_request(""), m_received(false)
{
		(void)*m_v_server;
		(void)m_socket;
		(void)m_request;
		(void)m_received;
}


Client::Client(int socket) 
	: m_v_server(0), m_socket(socket), m_request(""), m_received(false)
{

}
