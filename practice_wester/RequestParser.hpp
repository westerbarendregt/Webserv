/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   RequestParser.hpp                                  :+:    :+:            */
/*                                                     +:+                    */
/*   By: wester <wester@student.codam.nl>             +#+                     */
/*                                                   +#+                      */
/*   Created: 2021/02/23 12:17:47 by wester        #+#    #+#                 */
/*   Updated: 2021/03/01 16:39:14 by wester        ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include <stdbool.h>
# include <vector>
# include <string>
# include <fstream>
# include <iostream>
# include <sstream>
# include <algorithm>

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
	"WWW-AUTHENTICATE"
};

static const char *protocol = "HTTP/1.1";

typedef struct  s_request
{
	std::vector<std::string> 			m_headers;

	int                     	m_method;
	char                        m_path[1024];
	int                  	    m_protocol;
	size_t						m_content_length;
	bool						done; 

}               t_request;

namespace RequestParser
{ 
	int            GetFirstLine(client )
	{
		std::string line;
		
		if (std::getline(stream, line))
		{
			for (int i = 0; methods[i]; ++i){
				if (!line.compare(0, line.find_first_of(BLANKS), methods[i])){
					data->m_method = i;
					size_t ret = line.copy(data->m_path, (line.find("HTTP") -1) -
					(line.find_first_of(BLANKS) + 1), line.find_first_of(BLANKS) + 1);
					data->m_path[ret] = 0;
					if (line.find(protocol) != std::string::npos){
						data->m_protocol = HTTP1;
						return SUCCESS;
					}
				}
			}
			return ERROR;    
		}
		return ERROR;
	}
	
	int				GetHeaders(std::istringstream& stream, t_request *data)
	{
		std::string line;
		
		for (int i = 0; i < 18; ++i)
			data->m_headers.push_back("");
		while (std::getline(stream, line)){
			for (int i = 0; headers[i]; ++i){
				for (int j = 0; line[j] != ':'; ++j)
					line[j] = std::toupper(line[j]);
				if (line.find(headers[i]) != std::string::npos)
					data->m_headers[i] = line.substr(line.find(": ") + 2, line.size() - (line.find(":") + 1));
			}
		}
		return SUCCESS;
	}
	
	void				ErrorRequest(int line)
	{
		if (line == 0)
			throw "Error on Request line 1";
		if (line == 1)
			throw "Error with Request headers";
	}

	void				CleanData(t_request *data)
	{
		data->m_protocol = -1;
		data->m_method = -1;
		memset(data->m_path, 0, 1024);// = (char *)"";
	}


	// RequestParser(){};
	// RequestParser(const RequestParser& other){
	// 	*this = other;
	// }
	// RequestParser& operator=(const RequestParser& other)
	// {
	// 	return *this;
	// }
	// ~RequestParser(){};
		
	void		Print(t_request *data)
	{
		switch (data->m_method)
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
		std::cout << "protocol: " << data->m_protocol << std::endl;
		std::cout << "path: " << data->m_path << std::endl;
		for (int i = 0; i < 18; ++i)
			std::cout << headers[i] << ":" << data->m_headers[i] << std::endl;
	}
	
	void		Parse(char *request, t_request& data)
	{
		size_t n = 0;
		
		std::istringstream stream;
		stream.str(request);
		CleanData(&data);	
		if (GetFirstLine(stream, &data))
			return ErrorRequest(0);
		if (GetHeaders(stream, &data))
			return ErrorRequest(1);
	}
	
};

#endif
