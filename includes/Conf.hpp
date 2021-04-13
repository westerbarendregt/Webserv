#ifndef CONF_HPP
# define CONF_HPP

#include <iostream>
#include <map>
#include <vector>
#include <string>

struct	s_prefix_comp {
	//some opti required for subsequent // and location that ends with /
	s_prefix_comp() {};
	bool	operator()(std::string const &lhs, std::string const &rhs) const {
		size_t	lhs_prefixes = 0;
		size_t	rhs_prefixes = 0;
		size_t	index = 0;

		while ((index = lhs.find('/', index)) != std::string::npos) {
			++index;
			++lhs_prefixes;
		}
		index = 0;
		while ((index = rhs.find('/', index)) != std::string::npos) {
			++index;
			++rhs_prefixes;
		}
		return (lhs_prefixes > rhs_prefixes
				|| (lhs_prefixes == rhs_prefixes && lhs > rhs));
	}
};

struct	s_v_server_conf
{
	public:
		typedef std::map<std::string, std::string> t_directives;
		typedef std::map<std::string, t_directives, s_prefix_comp> t_routes;
		typedef std::vector<std::string> t_error_pages;
		typedef t_routes::value_type 				t_route;
		typedef	t_routes::key_type					t_route_index;
		s_v_server_conf() {this->m_directives["listen"] = "80";}
		t_directives				m_directives;
		t_routes					m_routes;
		t_error_pages				m_error_pages;
		std::vector<t_route_index>	m_route_indexes;
};

#endif

