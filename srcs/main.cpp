#define DEFAULT_PATH "./conf/webserv.conf"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "Server.hpp"
#include "Error.hpp"
#include "utils.hpp"
#include "Logger.hpp"


Server s;

int	usage() {
	std::cerr<< "usage: webserv [conf_file]" <<std::endl;
	return (ERROR);
}

void	sigtermClose(int) {
	s.close();
	std::cerr << "\nexiting webserv..."<< std::endl;
	exit(128 + SIGINT);
}

int	main(int ac, char **av) {

	if (ac > 2)
		return (usage());
	try
	{
		if (signal(SIGPIPE, SIG_IGN) == SIG_ERR
				|| signal(SIGINT, &sigtermClose) == SIG_ERR
				|| signal(SIGTERM, &sigtermClose) == SIG_ERR)
			throw serverError("main: signal:", strerror(errno));

		char	const *path = ac == 2 ? av[1] : DEFAULT_PATH;

		s.parse(path);
		s.init();
		s.run();
	}
	catch(std::exception &e)
	{
		std::cerr << "fatal error: " << e.what() <<std::endl;
		s.close();
		return (ERROR);
	}
	return (SUCCESS);
}
