#define DEFAULT_PATH "./webserv.conf"
#include <iostream>
#include "Server.hpp"

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
		Server s(path);
		//s.run();
	}
	catch(std::exception &e)
	{
		std::cerr<< "error: " << e.what() <<std::endl;
		return (1);
	}
	return (0);
}
