#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>

namespace base64 
{
std::string encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string decode(std::string const& encoded_string);
}

namespace ft
{
uint16_t    hostToNetworkShort(uint16_t hostshort);
uint32_t    hostToNetworkLong(uint32_t hostlong);
std::string inet_ntoa(struct in_addr &in);
bool 	    isBigEndian();

size_t		Atoi(const char *str);
size_t		AtoiHex(const char *str);
std::string sputnbr(size_t n);

int		    getline_crlf(std::string& total, std::string& line, int line_break, size_t& start);
int		    getline(std::string& total, std::string& line, int line_break, size_t& start);
int	        getNextLine(int fd, std::string &line);

std::string	convertDate(const time_t * clock);
size_t	    fullMetaData(std::string const & src);

bool	                    compare(char c, char *str);
std::vector<std::string>	split(std::string const & s, char delim = ' ');
char	                    *strdup(std::string const & src);
std::string                 intToString(int n);
std::string                 hexString(size_t	n);
}

#endif
