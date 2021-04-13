#include <ctime>
#include <string>//std::string
#include <string.h>
#include <unistd.h>//size_t
#include <stdlib.h>//malloc
#include <stdint.h>
#include <limits.h>
#include <iostream>
#include <vector>
#include <errno.h>
#include "Error.hpp"
#include "WebServer.hpp"


namespace ft {
static size_t		write_reverse(size_t n, char *buf)
{
	size_t	len;

	len = 0;
	while (n)
	{
		buf[len] = '0' + n % 10;
		n *= 0.1;
		len++;
	}
	return (len);
}

std::string sputnbr(size_t n)
{
	size_t	start;
	size_t	end;
	char	buf[20];
	char	swap;

	std::fill(buf, buf + 20, 0);
	end = 0;
	if (!n)
	{
		buf[0] = '0';
		buf[1] = '\0';
		return (std::string(buf));
	}
	start = write_reverse(n, buf);
	buf[start] = '\0';
	start--;
	while (start > end)
	{
		swap = buf[end];
		buf[end] = buf[start];
		buf[start] = swap;
		start--;
		end++;
	}
	return (std::string(buf));
}


static int	atoiIsBlank(char c)
{
	return (c == '\t' || c == '\v' || c == '\f'
				|| c == '\r' || c == '\n' || c == ' ');
}

static bool		checkHex(char c1)
{
	char c = tolower(c1);
	if (c == 'a' || c == 'b' || c == 'c')
		return true;
	if (c == 'd' || c == 'e' || c == 'f')
		return true;
	return false;
}	

static int		makeDecimal(char c1)
{
	int i = 0;
	if (c1 >= 48 && c1 <= 57)
		return c1 - '0';
	char c = tolower(c1);
	for (; c - i != 'a'; ++i);
	return 10 + i;
}

static int		atoiConvertHex(const char *str, int sign)
{
	long int	result;
	int			i;

	result = 0;
	i = 0;
	while ((str[i] >= 48 && str[i] <= 57) || checkHex(str[i]))
	{
		if (result && LONG_MAX / result == 16)
			return (((int)result * 16 + (str[i] - '0')) * sign);
		if (result && LONG_MAX / result < 16 && sign == -1)
			return (0);
		if (result && LONG_MAX / result < 16 && sign == 1)
			return (-1);
		int dec = makeDecimal(str[i]);
		result = result * 16 + dec;
		i++;
	}
	return (((int)result) * sign);
}

size_t		AtoiHex(const char *str)
{
	int			i;
	int			sign;

	i = 0;
	sign = 1;
	while (atoiIsBlank(str[i]))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	return (atoiConvertHex(str + i, sign));
}

static int	atoiConvert(const char *str, int sign)
{
	long int	result;
	int			i;

	result = 0;
	i = 0;
	while (str[i] >= 48 && str[i] <= 57)
	{
		if (result && LONG_MAX / result == 10)
			return (((int)result * 10 + (str[i] - '0')) * sign);
		if (result && LONG_MAX / result < 10 && sign == -1)
			return (0);
		if (result && LONG_MAX / result < 10 && sign == 1)
			return (-1);
		result = result * 10 + (str[i] - '0');
		i++;
	}
	return (((int)result) * sign);
}

size_t			Atoi(const char *str)
{
	int			i;
	int			sign;

	i = 0;
	sign = 1;
	while (atoiIsBlank(str[i]))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			sign = -1;
		i++;
	}
	return (atoiConvert(str + i, sign));
}

bool isBigEndian() {
	union {
		uint32_t i;
		uint8_t	 c[4];
	} u = {0x01020304};

	return u.c[0] == 1;
}

uint32_t hostToNetworkLong(uint32_t hostlong) {
	if (isBigEndian())
		return hostlong;
	return (hostlong & 0x000000FF) << 24 |
		   (hostlong & 0x0000FF00) << 8 |
		   (hostlong & 0x00FF0000) >> 8 |
		   (hostlong & 0xFF000000) >> 24;
}

uint16_t hostToNetworkShort(uint16_t hostshort) {
	if (isBigEndian())
		return hostshort;
	return (hostshort & 0x00FF) << 8 | (hostshort & 0xFF00) >> 8;
}

int		getline_crlf(std::string& total, std::string& line, int line_break, size_t& start) // make line_break 1 if you want it in, zero if you don't
{
	size_t len;

	if (total.find(CRLF, start) != std::string::npos)
	{
		len = total.find(CRLF, start) - start;
		line = total.substr(start, len + (line_break * 2));
		start = total.find(CRLF, start) + 2;
		return 1;
	}
	else {
		line = total.substr(start, total.size() - start);
		// start = 0;
	}
	return 0;
}

int		getline(std::string& total, std::string& line, int line_break, size_t& start) // make line_break 1 if you want it in, zero if you don't
{
	size_t len;

	if (total.find('\n', start) != std::string::npos)
	{
		len = total.find('\n', start) - start;
		line = total.substr(start, len + line_break);
		start = total.find('\n', start) + 1;
		return 1;
	}
	else {
		line = total.substr(start, total.size() - start);
		// start = 0;
	}
	return 0;
}

int	getNextLine(int fd, std::string& line)  { // lol
	static char read_buf[5000];
	static std::string saved;
	size_t	found = saved.find('\n', 0);
	static ssize_t	bytes = -1;

	if (found != std::string::npos || !bytes) {
		line.clear();
		line = saved.substr(0, found);
		saved.erase(0, found + 1);
		if (!bytes) {
			bytes = -1;
			return 0;
		}
		return 1;
	}
	std::fill(read_buf, read_buf + sizeof(read_buf), 0);
	while ((bytes = read(fd, read_buf, 4096)) > 0) {
		saved.append(read_buf);
		std::fill(read_buf, read_buf + sizeof(read_buf), 0);
		found = saved.find('\n', 0);
		if (found != std::string::npos) {
			line.clear();
			line = saved.substr(0, found);
			saved.erase(0, found + 1);
			return 1;
		}
	}
	if (bytes == -1)
		throw serverError("getNextLine: read:", strerror(errno));
	return getNextLine(fd, line);
}

bool	compare(char c, char *str)
{
	for (int i = 0; str[i]; ++i)
		if (str[i] == c)
			return true;
	return false;
}

char	*strdup(std::string const & src) {
	char *result = reinterpret_cast<char *>(malloc(src.size() + 1));
	if (!result)
		return NULL;
	size_t	len = src.copy(result, src.size(), 0);
	result[len] = '\0';
	return result;
}

std::string intToString(int n) {
	if (n == 0) {
		return "0";
	}

	std::string s;

	while (n) {
		s = static_cast<char>((n % 10) + '0') + s;
		n /= 10;
	}
	return s;
}

std::string	convertDate(const time_t * clock) {
	char s[1025] = {};
	struct tm*	timeptr = gmtime(clock);

	strftime(s, 1024, "%a, %d %b %Y %T GMT", timeptr);
	return s;
}

std::string hexString(size_t n) {
	std::string s;

	if (n == 0)
		return "0";
	while (n) {
		s = HEX_STR[n % 16] + s;
		n /= 16;
	}
	return s;
}

size_t	fullMetaData(std::string const &src) {
	return src.find("\r\n\r\n");
}

std::vector<std::string>	split(std::string const & s, char delim) {
	std::vector<std::string>	v;
	std::size_t	pos, prev = 0;

	do {
		pos = s.find(delim, prev);
		std::string	token = s.substr(prev, (pos == std::string::npos) ? std::string::npos : pos - prev);
		if (!token.empty()) v.push_back(token);
		prev = pos + 1;
	} while (pos != std::string::npos);

	return v;
}

std::string inet_ntoa(struct in_addr &in)
{
  unsigned char *bytes = reinterpret_cast<unsigned char *>(&in);
  return intToString(bytes[0])
	  + "." + intToString(bytes[1])
	  + "." + intToString(bytes[2])
	  + "." + intToString(bytes[3]);
}
}
