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
		friend	class	Server;
		Cgi();
		void	run(t_client &c);
		~Cgi();
	private:
		void	init();
		void	fillEnv(t_request_data &request);
		void	convertEnv(t_client &c);
		void	setParentIo(t_client &c);
		void	setChildIo(t_client &c);
		void	setCgiFd(fd_set *read_set, fd_set *write_set, t_client &c);
		void	exec(t_client &c);
		void	read(t_client &c);
		void	write(t_client &c);
		void	generateResponse(t_client &c);
		void	closeAll(t_client &c);
		void	kill(t_client &c);
		void	reset();
		void	reset(t_client &c);
		void	clear();
		t_cgi_env_map		m_env_map;
		t_cgi_env_array 	m_env_array;
		t_cgi_argv			m_argv;
};

#endif
