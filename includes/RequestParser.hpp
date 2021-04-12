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
# include "WebServer.hpp"
# include "Logger.hpp"

#define BLANKS "\t\v\n "

class Client;

static const char *methods[] = {"GET", "HEAD", "POST", "PUT", "DELETE", NULL};

static const char *headers[] = {"ACCEPT-CHARSETS", "ACCEPT-LANGUAGE", "ALLOW", "AUTHORIZATION",
	"CONTENT-LANGUAGE", "CONTENT-LENGTH", "CONTENT-LOCATION", "CONTENT-TYPE", "DATE", "HOST",
	"LAST-MODIFIED", "LOCATION", "REFERER", "RETRY-AFTER", "SERVER", "TRANSFER-ENCODING", "USER-AGENT",
	"WWW-AUTHENTICATE", NULL};

static const char *protocol = "HTTP/1.1";

class RequestParser
{ 
	static int            GetFirstLine(Client& c)
	{
		std::string line;
		
		if (ft::getline_crlf(c.m_request_str, line, 0, c.m_request_data.m_start))
		{
			for (int i = 0; methods[i]; ++i){
				if (!line.compare(0, line.find_first_of(BLANKS), methods[i])){
					c.m_request_data.m_method = i;
					int first = line.find_first_of(' ') + 1;
					int last = line.find_last_of(' ');
					if (line.find_last_of(' ') != line.find_first_of(' ', first)){
						c.m_request_data.m_status_code = 400; // no whitespaces aloud in startline except two seperating single space // maybe a 404
						return ERROR;
					}
					c.m_request_data.m_path = line.substr(first, last - first);
					if (line.find(protocol) != std::string::npos){
						c.m_request_data.m_protocol = HTTP1;
						return SUCCESS;
					}
					c.m_request_data.m_status_code = 400; // Bad Request (not HTTP/1.1)
				}
			}
			c.m_request_data.m_status_code = 501; // method not implemented (or no method found)
			return ERROR;
		}
		c.m_request_data.m_status_code = 400; // Bad Request (not HTTP/1.1)
		return ERROR;
	}
	
	static std::string				getValueHeader(std::string& line)
	{
		std::string header = line.substr(line.find(':') + 1, line.size() - (line.find(":") + 1));
		size_t start = header.find_first_not_of(BLANKS);
		size_t len = header.find_last_not_of(BLANKS) - start + 1;
		return header.substr(start, len); //trimming start and end of whitespaces
	}

	static int				GetHeaders(Client& c)
	{
		std::string line;
		bool first = true;

		while (ft::getline_crlf(c.m_request_str, line, 0, c.m_request_data.m_start))
		{
			if (ft::compare(line[0], (char*)BLANKS)){
				c.m_request_data.m_status_code = 400; // Bad request (blanks between start line and first header)
				return ERROR;
			}
			if (line[0] == 0)
				return SUCCESS;
			first = false;
			for (int j = 0; line[j] != ':' && line[j]; ++j){
				line[j] = std::toupper(line[j]);
				if (ft::compare(line[j], (char*)BLANKS)){
					c.m_request_data.m_status_code = 400; // Bad request (blanks before colon)
					return ERROR;
				}
			}
			for (int i = 0; headers[i]; ++i){
				if (line.find(headers[i]) != std::string::npos)
					c.m_request_data.m_headers[i] = getValueHeader(line);
			}
			if (line[0] == 'X'&& line[1] == '-' && line.find(":") != std::string::npos){
				std::string key = line.substr(0, line.find(":"));
				std::string value = getValueHeader(line);
    			c.m_request_data.x_headers[key] = value;
			}
		}
		return SUCCESS;
	}
	
	
	static int				ErrorRequest(Client &c, int line)
	{
		c.m_request_data.m_start = 0;
		c.m_request_data.m_done = true;
		if (line == 0)
			Logger::Log() <<  "Error on Request line 1" << std::endl;
		if (line == 1)
			Logger::Log() <<  "Error with Request headers" << std::endl;
		if (line == 2)
			Logger::Log() <<  "Error with Body" << std::endl;
		return ERROR;
	}

	static void				CleanData(Client& c)
	{
		c.m_request_data.m_method = -1;
		c.m_request_data.m_path.clear();
		c.m_request_data.m_protocol = -1;
		c.m_request_data.m_content_length = 0;
		c.m_request_data.m_headers.clear();
		c.m_request_data.m_headers.assign(18, "");
		c.m_request_data.m_if_body = false;
		c.m_request_data.m_body.clear();
		c.m_request_data.m_done = false;
		c.m_request_data.m_metadata_parsed = false;
		c.m_request_data.m_chunked = false;
		c.m_request_data.m_status_code = 0;
		c.m_request_data.m_cgi = 0;
		c.m_request_data.m_start = 0;

	}

	static void				CheckHeaderData(Client& c)
	{
		c.m_request_data.m_content_length = ft::Atoi(c.m_request_data.m_headers[CONTENTLENGTH].c_str());
		if (c.m_request_data.m_headers[TRANSFERENCODING].find("chunked") != std::string::npos){
			c.m_request_data.m_chunked = true;
			c.m_request_data.m_if_body = true;
		}
		if (c.m_request_data.m_content_length > 0)
			c.m_request_data.m_if_body = true;
	}

	static bool					ChunkedData(Client& c, bool first)
	{
		std::string line;
		int ret = 1;

		if (first)
			c.m_request_str = c.m_request_str.substr(c.m_request_data.m_start);
		c.m_request_data.m_tmp_body.append(c.m_request_str);
		if (c.m_request_data.m_tmp_body.find("0\r\n\r\n") == std::string::npos)
			return SUCCESS;
		c.m_request_data.m_start = 0;
		while (ret)
		{
			ret = ft::getline_crlf(c.m_request_data.m_tmp_body, line, 1, c.m_request_data.m_start);
			if ((line == "\r\n") && c.m_request_data.m_last_chunk == true)
			{
					c.m_request_data.m_done = true;
					if (c.m_request_data.m_body.size() == c.m_request_data.m_content_length)
						return SUCCESS;
					return ERROR;
			}
			c.m_request_data.m_last_chunk = false;
			if (!ret){
				c.m_request_data.m_body.append(line);
				return SUCCESS;
			}
			if (c.m_request_data.m_looking_for_size == true){
				c.m_request_data.m_looking_for_size = false;
				size_t current_chunk_size = ft::AtoiHex(line.c_str());	
				if (current_chunk_size == 0 && line == "0\r\n")
					c.m_request_data.m_last_chunk = true;
				if (c.m_request_data.m_body.size() != c.m_request_data.m_content_length){
					std::cout << "size: " << c.m_request_data.m_body.size() << " length: " << c.m_request_data.m_content_length << std::endl;
					return ERROR;
				}
				c.m_request_data.m_content_length += current_chunk_size;
			}
			else {
				c.m_request_data.m_looking_for_size = true;
				c.m_request_data.m_body.append(line.substr(0, line.size() - 2));
			}
		}
		Logger::Log() << "this should not happen" << std::endl;
		return SUCCESS;
	}

  public:

	static std::string 		GetMethodString(Client &c)
	{
		switch (c.m_request_data.m_method)
		{
			case GET : return "GET"; 
			case HEAD : return "HEAD"; 
			case POST : return "POST"; 
			case PUT : return "PUT"; 
			case DELETE : return "DELETE";
		}
		return "No match";
	}

	static void				ResetClient(Client &c)
	{
		c.m_request_str.clear();
		CleanData(c);
	}

	static int				GetBody(Client& c, bool first)
	{
		std::string line;
		int ret = 1;

		if (c.m_request_data.m_chunked == true)
		{
			if (ChunkedData(c, first)){
				c.m_request_data.m_start = 0;
				c.m_request_str.clear();
				c.m_request_data.m_status_code = 400;
				c.m_request_data.m_done = true;
				return ERROR;
			}
			c.m_request_data.m_start = 0;
			c.m_request_str.clear();
			return SUCCESS;
		}
		else if (c.m_request_data.m_chunked == false){
			while (ret){
				ret = ft::getline_crlf(c.m_request_str, line, 1, c.m_request_data.m_start);
				c.m_request_data.m_body.append(line);
			}
		}
		if (c.m_request_data.m_body.size() >= c.m_request_data.m_content_length)
		{
			c.m_request_data.m_done = true;
			c.m_request_data.m_start = 0;
			if (c.m_request_data.m_body.size() > c.m_request_data.m_content_length){
				c.m_request_data.m_status_code = 400;
				return ERROR;
			}
		}
		else 
			c.m_request_data.m_done = false;
		c.m_request_str.clear();
		return SUCCESS;
	}

	static void		Print(Client& c)
	{
		switch (c.m_request_data.m_method)
		{
			case GET : Logger::Log() << "method: " << "GET" << std::endl;
				break ;
			case HEAD : Logger::Log() << "method: " << "HEAD" << std::endl;
				break ;
			case POST : Logger::Log() << "method: " << "POST" << std::endl;
				break ;
			case PUT : Logger::Log() << "method: " << "PUT" << std::endl;
				break ;
			case DELETE : Logger::Log() << "method: " << "DELETE" << std::endl;
				break ;
		}
		Logger::Log() << "path: " << c.m_request_data.m_path << std::endl;
		switch (c.m_request_data.m_protocol)
		{
			case HTTP1 : Logger::Log() << "protocol: " << "HTTP/1.1" << std::endl;
				break ;
		}
		for (int i = 0; i < 18; ++i)
			Logger::Log() << headers[i] << ":" << c.m_request_data.m_headers[i] << std::endl;//"---" << c.m_request_data.m_headers[i].size() << std::endl;
		for(std::map<std::string, std::string>::iterator it = c.m_request_data.x_headers.begin(); it != c.m_request_data.x_headers.end(); ++it)
			Logger::Log() << "key = [" << it->first << "] value = [" << it->second << "]" << std::endl;
		if (c.m_request_data.m_chunked)
			Logger::Log() << std::endl << "The body is chunked!" << std::endl;
		Logger::Log() << "BODY-length: " << c.m_request_data.m_content_length << std::endl;
		// Logger::Log() << "BODY:" << c.m_request_data.m_body << std::endl << std::endl;
		if (c.m_request_data.m_status_code)
			Logger::Log() << "While parsing found error NR: " << c.m_request_data.m_status_code << std::endl;
		if (c.m_request_data.m_metadata_parsed && c.m_request_data.m_done)
			Logger::Log() << "METADATA IS PARSED AND BODY PARSING IS DONE" << std::endl << std::endl;
		else if (c.m_request_data.m_metadata_parsed)
			Logger::Log() << "METADATA PARSED BUT BODY PARSING IS NOT FINISHED" << std::endl << std::endl;
		else
			Logger::Log() << "REQUEST DATA NOT PARSED" << std::endl << std::endl;
		
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
			if (GetBody(c, true))
				return ErrorRequest(c, 2);
		}
		else
			c.m_request_data.m_done = true;
		c.m_request_data.m_start = 0;
		return SUCCESS;
	}
};

#endif
