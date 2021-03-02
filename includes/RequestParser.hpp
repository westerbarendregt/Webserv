/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestParser.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/23 12:17:47 by wester        #+#    #+#                 */
/*   Updated: 2021/03/02 22:59:40 by wester        ########   odam.nl         */
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

#define BLANKS "\t\v "

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
		
		if (ft_getline(c.m_request, line, 0))
		{
			for (int i = 0; methods[i]; ++i){
				if (!line.compare(0, line.find_first_of(BLANKS), methods[i])){
					c.m_data.m_method = i;
					size_t ret = line.copy(c.m_data.m_path, (line.find("HTTP") -1) -
					(line.find_first_of(BLANKS) + 1), line.find_first_of(BLANKS) + 1);
					c.m_data.m_path[ret] = 0;
					if (line.find(protocol) != std::string::npos){
						c.m_data.m_protocol = HTTP1;
						return SUCCESS;
					}
				}
			}
			return ERROR;    
		}
		return ERROR;
	}
	
	static int				GetHeaders(Client& c)
	{
		std::string line;
		
		while (ft_getline(c.m_request, line, 0))
		{
			if (line[0] == 0)
				return SUCCESS;
			for (int j = 0; line[j] != ':' && line[j]; ++j)
				line[j] = std::toupper(line[j]);
			for (int i = 0; headers[i]; ++i){
				if (line.find(headers[i]) != std::string::npos)
					c.m_data.m_headers[i] = line.substr(line.find(": ") + 2, line.size() - (line.find(":") + 1));
			}
		}
		return SUCCESS;
	}
	
	static int				GetBody(Client& c)
	{
		std::string line;
		int ret = 1;
		
		c.m_data.m_if_body = true;
		while (ret){
			ret = ft_getline(c.m_request, line, 1);
			c.m_data.m_body.append(line);
		}
		if (c.m_data.m_body.size() == c.m_data.m_content_length)
			c.m_data.m_done = true;
		else 
			c.m_data.m_done = false;
		std::cout << "---------!!: " << c.m_data.m_body.size() << std::endl;
		return SUCCESS;
	}
	
	static void				ErrorRequest(int line)
	{
		if (line == 0)
			throw "Error on Request line 1";
		if (line == 1)
			throw "Error with Request headers";
		if (line == 2)
			throw "Error with Body";
	}

	static void				CleanData(Client& c)
	{
		c.m_data.m_method = -1;
		memset(c.m_data.m_path, 0, 1024);
		c.m_data.m_protocol = -1;
		c.m_data.m_content_length = 0;
		c.m_data.m_headers.clear();
		c.m_data.m_if_body = false;
		c.m_data.m_body = "";
		c.m_data.m_done = false;
	}
  public:
	static void		Print(Client& c)
	{
		switch (c.m_data.m_method)
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
		switch (c.m_data.m_protocol)
		{
			case HTTP1 : std::cout << "protocol: " << "HTTP/1.1" << std::endl;
				break ;
		}
		std::cout << "path: " << c.m_data.m_path << std::endl;
		for (int i = 0; i < 18; ++i)
			std::cout << headers[i] << ":" << c.m_data.m_headers[i] << std::endl;
		std::cout << std::endl << "BODY-length: " << c.m_data.m_content_length << std::endl;
		std::cout << "BODY:" << c.m_data.m_body << std::endl << std::endl;
		if (c.m_data.m_done)
			std::cout << "PARSING IS DONE" << std::endl;
		else 
			std::cout << "PARSING IS NOT FINISHED" << std::endl;
	}
	
	static void		Parse(Client& c)
	{
		if (c.m_data.m_if_body == true && c.m_data.m_done == false){
			if (GetBody(c))
				return ErrorRequest(2);
			return ;	
		}
		CleanData(c);	
		if (GetFirstLine(c))
			return ErrorRequest(0);
		if (GetHeaders(c))
			return ErrorRequest(1);
		if (c.m_data.m_headers[CONTENTLENGTH][0] != 0 && (c.m_data.m_method == POST || c.m_data.m_method == PUT))
		{
			c.m_data.m_content_length = ftAtoi(c.m_data.m_headers[CONTENTLENGTH].c_str());
			if (GetBody(c))
				return ErrorRequest(2);
		}
		else 
			c.m_data.m_done = true;
	}
};

#endif
