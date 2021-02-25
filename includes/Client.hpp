#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include "VirtualServer.hpp"

class	Client // or connection
{
	public:
		friend class Server;
		explicit Client(VirtualServer *v_server, int socket);
		Client(int socket); // not sure how we link a client connection to a configuration yet

	private:
		VirtualServer	*m_v_server;
		int		m_socket;
		std::string	m_request;
		bool m_received;
};

#endif
