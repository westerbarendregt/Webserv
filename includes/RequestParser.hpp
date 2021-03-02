/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestParser.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/23 12:17:47 by wester        #+#    #+#                 */
/*   Updated: 2021/03/02 17:29:30 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include <stdbool.h>
# include <vector>
# include <string>
# include <iostream>
# include <algorithm>

# include "Client.hpp"
# include "utils.hpp"

# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>

# include <unistd.h>

#define BLANKS "\t\v "

std::string sputnbr(size_t n);

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

static const char *methods[] = {"GET", "HEAD", "POST", "PUT", "DELETE"};

static const char *headers[] = {"ACCEPT-CHARSETS", "ACCEPT-LANGUAGE", "ALLOW", "AUTHENTIZATION",
	"CONTENT-LANGUAGE", "CONTENT-LENGTH", "CONTENT-LOCATION", "CONTENT-TYPE", "DATE", "HOST",
	"LAST-MODIFIED", "LOCATION", "REFERER", "RETRY-AFTER", "SERVER", "TRANSFER-ENCODING", "USER-AGENT",
	"WWW-AUTHENTICATE", NULL
};

static const char *protocol = "HTTP/1.1";

namespace RequestParser
{ 
	int            GetFirstLine(int fd, Client& c)
	{
		std::string line;
		
		if (ft_getline(fd, line))
		{
			for (int i = 0; methods[i]; ++i){
				if (!line.compare(0, line.find_first_of(BLANKS), methods[i])){
					c.m_method = i;
					size_t ret = line.copy(c.m_path, (line.find("HTTP") -1) -
					(line.find_first_of(BLANKS) + 1), line.find_first_of(BLANKS) + 1);
					c.m_path[ret] = 0;
					if (line.find(protocol) != std::string::npos){
						c.m_protocol = HTTP1;
						return SUCCESS;
					}
				}
			}
			return ERROR;    
		}
		return ERROR;
	}
	
	int				GetHeaders(int fd, Client& c)
	{
		std::string line;
		
		for (int i = 0; i < 18; ++i)
			c.m_headers.push_back("");
		while (ft_getline(fd, line))
		{
			if (line[0] == 0)
				return SUCCESS;
			for (int j = 0; line[j] != ':' && line[j]; ++j)
				line[j] = std::toupper(line[j]);
			for (int i = 0; headers[i]; ++i){
				if (line.find(headers[i]) != std::string::npos)
					c.m_headers[i] = line.substr(line.find(": ") + 2, line.size() - (line.find(":") + 1));
			}
		}
		return SUCCESS;
	}
	
	int				GetBody(int fd, Client& c)
	{
		std::string line;
		int ret = 1;
		
		c.m_if_body = true;
		while (ret > 0)
		{
			ret = ft_getline(fd, line);
			c.m_body.append(line);
			if (ret == 2)
				c.m_body.append("\n");
		}
		if (c.m_body.size() == c.m_content_length)
			c.m_done = true;
		else 
			c.m_done = false;
		return SUCCESS;
	}
	
	void				ErrorRequest(int line)
	{
		if (line == 0)
			throw "Error on Request line 1";
		if (line == 1)
			throw "Error with Request headers";
		if (line == 2)
			throw "Error with Body";
	}

	void				CleanData(Client& c)
	{
		c.m_method = -1;
		memset(c.m_path, 0, 1024);
		c.m_protocol = -1;
		c.m_content_length = 0;
		c.m_headers.clear();
		c.m_if_body = false;
		c.m_body = "";
		c.m_done = false;
	}

	void		Print(Client& c)
	{
		switch (c.m_method)
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
		switch (c.m_protocol)
		{
			case HTTP1 : std::cout << "protocol: " << "HTTP/1.1" << std::endl;
				break ;
		}
		std::cout << "path: " << c.m_path << std::endl;
		for (int i = 0; i < 18; ++i)
			std::cout << headers[i] << ":" << c.m_headers[i] << std::endl;
		std::cout << std::endl << "BODY-length: " << c.m_content_length << std::endl;
		std::cout << "BODY:" << c.m_body << std::endl << std::endl;
		if (c.m_done)
			std::cout << "PARSING IS DONE" << std::endl;
		else 
			std::cout << "PARSING IS NOT FINISHED" << std::endl;
	}
	
	void		Parse(Client& c)
	{
		int fd = open("parse.txt", O_RDWR | O_CREAT | O_TRUNC, 0644);
		write(fd, c.m_request.c_str(), c.m_request.size());
		close(fd);
		fd = open("parse.txt", O_RDONLY); 
		
		if (c.m_if_body == true && c.m_done == false){
			if (GetBody(fd, c))
				return ErrorRequest(2);
			return ;	
		}
		CleanData(c);	
		if (GetFirstLine(fd, c))
			return ErrorRequest(0);
		if (GetHeaders(fd, c))
			return ErrorRequest(1);
		if (c.m_headers[CONTENTLENGTH][0] != 0 && (c.m_method == POST || c.m_method == PUT))
		{
			c.m_content_length = ftAtoi(c.m_headers[CONTENTLENGTH].c_str());
			if (GetBody(fd, c))
				return ErrorRequest(2);
		}
		else 
			c.m_done = true;
		close(fd);
	}
	
}

#endif
