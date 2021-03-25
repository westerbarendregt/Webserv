#define DEFAULT_PATH "./conf/webserv.conf"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "Server.hpp"
#include "Error.hpp"
#include "utils.hpp"

int	usage() {
	std::cerr<< "usage: webserv [conf_file]" <<std::endl;
	return (1);
}

int	main(int ac, char **av) {
		if (ac > 2)
		return (usage());

	char	const *path = ac == 2 ? av[1] : DEFAULT_PATH;


	try
	{
		if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
			throw serverError("main: signal:", strerror(errno));
		}
		Server s(path);
		s.init();
		s.run();
		s.close();
	}
	catch(std::exception &e)
	{
		std::cerr<< "error: " << e.what() <<std::endl;
		return (1);
	}
	return (0);
}
