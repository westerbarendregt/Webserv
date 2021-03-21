#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>

uint16_t    hostToNetworkShort(uint16_t hostshort);
uint32_t    hostToNetworkLong(uint32_t hostlong);
bool 	    isBigEndian();
int		    ftAtoi(const char *str);
std::string sputnbr(size_t n);
int		    ft_getline_crlf(std::string& total, std::string& line, int line_break, size_t& start);
int		    ft_getline(std::string& total, std::string& line, int line_break, size_t& start);
bool	    ft_compare(char c, char *str);
std::string intToString(int n);
std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len);
std::string base64_decode(std::string const& encoded_string);

char	*ft_strdup(std::string &src);
size_t	fullMetaData(std::string const & src);
std::string hexString(size_t	n);

#endif
