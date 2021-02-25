#ifndef CONF_HPP
# define CONF_HPP

#include <iostream>
#include <map>
#include <vector>
#include <string>

struct	s_v_server_conf
{
	public:
		typedef std::map<std::string, std::string> t_directives;
		typedef std::map<std::string, t_directives> t_routes;
		typedef t_routes::value_type 				t_route;
		typedef	t_routes::key_type					t_route_index;
		s_v_server_conf() {this->m_directives["listen"] = "80";}
		t_directives				m_directives;
		t_routes					m_routes;
		std::vector<t_route_index>	m_route_indexes;
};

#endif

