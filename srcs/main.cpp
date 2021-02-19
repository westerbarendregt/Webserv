#define DEFAULT_PATH "./webserv.conf"
#include <iostream>
# include "ConfigParser.hpp"

int	usage() {
	std::cerr<< "usage: webserv [conf_file]" <<std::endl;
	return (ERROR);
}

int	main(int ac, char **av) {
	if (ac > 2)
		return (usage());

	char	const *path = ac == 2 ? av[1] : DEFAULT_PATH;

	try
	{
		std::queue<serverToken> tokens;
		ConfigParser::parse(path, tokens);
		ConfigParser::printServer(tokens);
	}
	catch(std::exception &e)
	{
		std::cerr<< "error: " << e.what() << std::endl;
		return (ERROR);
	}
	return (0);
}
