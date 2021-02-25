#ifndef VIRTUAL_SERVER_HPP
# define VIRTUAL_SERVER_HPP

#include "Conf.hpp"


class	VirtualServer
{
	public:
		typedef	s_v_server_conf	t_v_server_conf;
		friend class Server;
		friend class Client;
		friend class ConfigParser;
		explicit VirtualServer(t_v_server_conf conf);
	private:
		int						m_socket;
		t_v_server_conf			m_configs;
};


#endif
