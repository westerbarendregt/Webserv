#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include "VirtualServer.hpp"

class	Client
{
	public:
		friend class Server;
		explicit Client(VirtualServer *v_server, int socket);
		Client(int socket);
		Client();
		bool	fullHttpRequest();

	private:
		VirtualServer	*m_v_server;
		int		m_socket;
		std::string	m_request;
		bool m_received;
		bool m_treated;
		struct	sockaddr_storage m_sockaddr;
		socklen_t	m_addrlen;

		//t_request	m_struct;
};

#endif
