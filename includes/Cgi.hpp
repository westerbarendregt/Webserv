#ifndef CGI_HPP
# define CGI_HPP

# include <map>
# include <string>
# include "Client.hpp"

# define CGI_ENV_LEN 19


class	Cgi {
	public:
		typedef	std::map<std::string, std::string>	t_cgi_env_map;
		typedef	char**								t_cgi_env_array;
		typedef	char**								t_cgi_argv;
		typedef	Client								t_client;
		typedef	t_client::t_request_data					t_request_data;

		friend	class	RequestHandler;
		Cgi();
		void	run(t_client &c);
		~Cgi();
	private:
		void	init();
		void	exec(t_client &c);
		void	read(t_client &c);
		void	write(t_client &c);
		void	fillEnv(t_request_data &request);
		void	convertEnv(t_client &c);
		void	reset();
		void	clear();
		t_cgi_env_map		m_env_map;
		t_cgi_env_array 	m_env_array;
		t_cgi_argv			m_argv;
};

#endif
