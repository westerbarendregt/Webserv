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

	private:
		VirtualServer	*m_v_server;
		int		m_socket;
		std::string	m_request;
		bool m_received;
		//t_request	m_struct;
};

#endif
