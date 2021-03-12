#ifndef CGI_HPP
# define CGI_HPP

# include <map>
# include <string>
# include "Client.hpp"

# define CGI_ENV_LEN 18


class	Cgi {
	public:
		typedef	std::map<std::string, std::string>	t_cgi_env;
		typedef	char**								t_cgi_exec_env;
		typedef	Client								t_client;
		typedef	t_client::t_request_data					t_request_data;

		friend	class	RequestHandler;
		Cgi();
		void	run(t_client &c);
		~Cgi();
	private:
		void	init();
		void	exec();
		void	fillEnv(t_request_data &request);
		void	convertEnv();
		t_cgi_env		m_env;
		t_cgi_exec_env 	m_av;
};

#endif
