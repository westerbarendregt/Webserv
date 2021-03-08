#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include <stdbool.h>
# include <vector>
# include <string>
#include <string.h>
# include <iostream>
# include <algorithm>

# include "Client.hpp"
# include "utils.hpp"

#define BLANKS "\t\v\n "
#define CHUNK  "\r\n"

class Client;

enum e_methods
{
	GET,
	HEAD,
	POST,
	PUT,
	DELETE
};

enum e_headers
{
	ACCEPTCHARSETS,
	ACCEPTLANGUAGE,
	ALLOW,
	AUTHENTIZATION,
	CONTENTLANGUAGE,
	CONTENTLENGTH,
	CONTENTLOCATION,
	CONTENTTYPE,
	DATE,
	HOST,
	LASTMODIFIED,
	LOCATION,
	REFERER,
	RETRYAFTER,
	SERVER,
	TRANSFERENCODING,
	USERAGENT,
	WWWAUTHENTICATE
};

enum e_protocol
{
	HTTP1 = 1
};

enum	e_status {
	INVALID = -1,
	SUCCESS = 0,
	ERROR = 1
};

static const char *methods[] = {"GET", "HEAD", "POST", "PUT", "DELETE", NULL};

static const char *headers[] = {"ACCEPT-CHARSETS", "ACCEPT-LANGUAGE", "ALLOW", "AUTHENTIZATION",
	"CONTENT-LANGUAGE", "CONTENT-LENGTH", "CONTENT-LOCATION", "CONTENT-TYPE", "DATE", "HOST",
	"LAST-MODIFIED", "LOCATION", "REFERER", "RETRY-AFTER", "SERVER", "TRANSFER-ENCODING", "USER-AGENT",
	"WWW-AUTHENTICATE", NULL};

static const char *protocol = "HTTP/1.1";

class RequestParser
{ 
	static int            GetFirstLine(Client& c)
	{
		std::string line;
		
		if (ft_getline(c.m_request_str, line, 0, c.m_request_data.m_start))
		{
			for (int i = 0; methods[i]; ++i){
				if (!line.compare(0, line.find_first_of(BLANKS), methods[i])){
					c.m_request_data.m_method = i;
					int first = line.find_first_of(' ') + 1;
					int last = line.find_last_of(' ');
					if (line.find_last_of(' ') != line.find_first_of(' ', first)){
						c.m_request_data.m_error = 400; // no whitespaces aloud in startline except two seperating single space // maybe a 404
						return ERROR;
					}
					c.m_request_data.m_path = line.substr(first, last - first);
					// size_t ret = line.copy(c.m_request_data.m_path, (line.find("HTTP") -1) -
					// (line.find_first_of(BLANKS) + 1), line.find_first_of(BLANKS) + 1);
					// c.m_request_data.m_path[ret] = 0;
					if (line.find(protocol) != std::string::npos){
						c.m_request_data.m_protocol = HTTP1;
						return SUCCESS;
					}
					c.m_request_data.m_error = 400; // Bad Request (not HTTP/1.1)
				}
			}
			c.m_request_data.m_error = 501; // method not implemented (or no method found)
			return ERROR;
		}
		c.m_request_data.m_error = 400; // Bad Request (not HTTP/1.1)
		return ERROR;
	}
	
	static int				GetHeaders(Client& c)
	{
		std::string line;
		bool first = true;

		while (ft_getline(c.m_request_str, line, 0, c.m_request_data.m_start))
		{
			if (ft_compare(line[0], (char*)BLANKS)){
				c.m_request_data.m_error = 400; // Bad request (blanks between start line and first header)
				return ERROR;
			}
			if (line[0] == 0)
				return SUCCESS;
			first = false;
			for (int j = 0; line[j] != ':' && line[j]; ++j){
				line[j] = std::toupper(line[j]);
				if (ft_compare(line[j], (char*)BLANKS)){
					c.m_request_data.m_error = 400; // Bad request (blanks before colon)
					return ERROR;
				}
			}
			for (int i = 0; headers[i]; ++i){
				if (line.find(headers[i]) != std::string::npos){
					std::string header = line.substr(line.find(':') + 1, line.size() - (line.find(":") + 1));
					size_t start = header.find_first_not_of(BLANKS);
					size_t len = (header.find_last_not_of(BLANKS) + 1) - start;
					c.m_request_data.m_headers[i] = header.substr(start, len); //trimming start and end of whitespaces
				}
			}
		}
		return SUCCESS;
	}
	
	
	static int				ErrorRequest(Client &c, int line)
	{
		c.m_request_data.m_start = 0;
		c.m_request_data.m_done = true;
		if (line == 0)
			std::cout <<  "Error on Request line 1" << std::endl;
		if (line == 1)
			std::cout <<  "Error with Request headers" << std::endl;
		if (line == 2)
			std::cout <<  "Error with Body" << std::endl;
		return ERROR;
	}

	static void				CleanData(Client& c)
	{
		c.m_request_data.m_method = -1;
		c.m_request_data.m_path.clear();
		c.m_request_data.m_protocol = -1;
		c.m_request_data.m_content_length = 0;
		c.m_request_data.m_headers.clear();
		c.m_request_data.m_if_body = false;
		c.m_request_data.m_body.clear();
		c.m_request_data.m_done = false;
		c.m_request_data.m_metadata_parsed = false;
		c.m_request_data.m_chunked = false;
		c.m_request_data.m_error = 0;
		c.m_request_data.m_start = 0;

	}

	static void				CheckHeaderData(Client& c)
	{
		// std::cout << "say hi \n"  << "hier: " << c.m_request_data.m_headers[TRANSFERENCODING] << std::endl;
		c.m_request_data.m_content_length = ftAtoi(c.m_request_data.m_headers[CONTENTLENGTH].c_str());
		if (c.m_request_data.m_headers[TRANSFERENCODING].find("chunked") != std::string::npos){
			c.m_request_data.m_chunked = true;
			c.m_request_data.m_if_body = true;
		}
		if (c.m_request_data.m_content_length > 0)
			c.m_request_data.m_if_body = true;
	}

	static bool					ChunkedData(Client& c)
	{
		std::string line;
		int ret = 1;

		// std::cout << c.m_request_str << std::endl;
			// std::cout << "hier:--------------- "<< line << std::endl;
		if (ft_getline(c.m_request_str, line, 1, c.m_request_data.m_start))
		{
			size_t current_chunk_size = ftAtoi(line.c_str());
			// std::cout << "hier!!!!!!!!!" << current_chunk_size << std::endl;
			if (current_chunk_size == 0 && line == "0\r\n")
			{
				c.m_request_data.m_done = true;
				if (c.m_request_data.m_body.size() != c.m_request_data.m_content_length)
					return SUCCESS;
				return ERROR;
			}
			if (line.find(CHUNK) != std::string::npos)
			{
				c.m_request_data.m_content_length += current_chunk_size;
				while (ret)
				{
					ret = ft_getline(c.m_request_str, line, 1, c.m_request_data.m_start);
					if (line.find(CHUNK) != std::string::npos){
						c.m_request_data.m_body.append(line.substr(0, line.size() - 2));
						// std::cout << "size:" << c.m_request_data.m_body.size() << "   other:" << c.m_request_data.m_content_length << std::endl;
						if (c.m_request_data.m_body.size() == c.m_request_data.m_content_length)
							return SUCCESS;
						// std::cout << "hier\n";
						return ERROR;
					}
					c.m_request_data.m_body.append(line);
				}
			}
		}
			// std::cout << "------------hallo\n";
		return ERROR;
	}

  public:

	static void				ResetClient(Client &c)
	{
		c.m_request_str.clear();
		CleanData(c);
	}

	static int				GetBody(Client& c)
	{
		std::string line;
		int ret = 1;

		// std::cout << "are we chunking?" << std::endl;
		if (c.m_request_data.m_chunked == true)
		{
			if (ChunkedData(c)){
				c.m_request_data.m_start = 0;
				return ERROR;
			}
			c.m_request_data.m_start = 0;
			return SUCCESS;
		}
		else if (c.m_request_data.m_chunked == false){
			while (ret){
				ret = ft_getline(c.m_request_str, line, 1, c.m_request_data.m_start);
				c.m_request_data.m_body.append(line);
			}
		}
		if (c.m_request_data.m_body.size() == c.m_request_data.m_content_length)
			c.m_request_data.m_done = true;
		else 
			c.m_request_data.m_done = false;
		c.m_request_data.m_start = 0;
		return SUCCESS;
	}

	static void		Print(Client& c)
	{
		switch (c.m_request_data.m_method)
		{
			case GET : std::cout << "method: " << "GET" << std::endl; 
				break ;
			case HEAD : std::cout << "method: " << "HEAD" << std::endl; 
				break ;
			case POST : std::cout << "method: " << "POST" << std::endl; 
				break ;
			case PUT : std::cout << "method: " << "PUT" << std::endl; 
				break ;
			case DELETE : std::cout << "method: " << "DELETE" << std::endl;
				break ;
		}
		switch (c.m_request_data.m_protocol)
		{
			case HTTP1 : std::cout << "protocol: " << "HTTP/1.1" << std::endl;
				break ;
		}
		std::cout << "path: " << c.m_request_data.m_path << std::endl;
		for (int i = 0; i < 18; ++i)
			std::cout << headers[i] << ":" << c.m_request_data.m_headers[i] << std::endl;
		std::cout << std::endl << "BODY-length: " << c.m_request_data.m_content_length << std::endl;
		std::cout << "BODY:" << c.m_request_data.m_body << std::endl << std::endl;
		if (c.m_request_data.m_done)
			std::cout << "PARSING IS DONE" << std::endl;
		else 
			std::cout << "PARSING IS NOT FINISHED" << std::endl;
	}
	
	static int		Parse(Client& c)
	{
		CleanData(c);
		c.m_request_data.m_metadata_parsed = true;
		if (GetFirstLine(c))
			return ErrorRequest(c, 0);
		if (GetHeaders(c))
			return ErrorRequest(c, 1);
		CheckHeaderData(c);
		if ((c.m_request_data.m_if_body || c.m_request_data.m_chunked ) && (c.m_request_data.m_method == POST || c.m_request_data.m_method == PUT))
		{
			if (GetBody(c))
				return ErrorRequest(c, 2);
		}
		else 
			c.m_request_data.m_done = true;
		return SUCCESS;
	}

	static	void	HandleBody(Client &) {
 			// adds to body, updates necessary information, compares lenght of body with Content-Length header field
			// if chunk, appends to body and searches for end chunk
			//updates c->m_request.done when detects end of body
			// it would also mark the request as done if body is complete
	}
};

#endif
