#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>

uint16_t    hostToNetworkShort(uint16_t hostshort);
uint32_t    hostToNetworkLong(uint32_t hostlong);
bool 	    isBigEndian();
int		    ftAtoi(const char *str);
std::string sputnbr(size_t n);
int		    ft_getline(std::string& total, std::string& line, int line_break, size_t& start);
bool	    ft_compare(char c, char *str);
char 		*ft_join(const char *s1, const char *s2, size_t len) ;
std::string intToString(int n);

#endif
