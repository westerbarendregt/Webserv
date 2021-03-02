#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>

uint16_t    hostToNetworkShort(uint16_t hostshort);
uint32_t    hostToNetworkLong(uint32_t hostlong);
bool 	    isBigEndian();
int		    ftAtoi(const char *str);
std::string sputnbr(size_t n);
int		    ft_getline(int fd, std::string& line);

#endif

