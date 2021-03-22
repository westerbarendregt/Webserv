#include <algorithm>
#include <iostream>
#include <string>

#include "Authentication.hpp"
#include "Conf.hpp"
#include "Error.hpp"
#include "RequestHandler.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "WebServer.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

RequestHandler::RequestHandler() {
	initStatusCodes();
	initMimeTypes();
}

RequestHandler::~RequestHandler() {}

std::string RequestHandler::handleAutoIndex() {
	struct	stat	buf;
	struct	dirent*	d;
	struct	tm*	timeptr;
	char 	s[1025] = {};
	std::string	body;

	DIR*	dir = opendir(this->m_client->m_request_data.m_real_path.c_str());
	if (dir == NULL) {
		throw HTTPError("opendir: ", "error opening directory", 500);
	}

	body += "<html>" CRLF;
	body += "<head><title>Index of /</title></head>" CRLF;
	body += "<body>" CRLF;
	body += "<h1>Index of ";
	body += this->m_client->m_request_data.m_path;
	body += "</h1><hr><pre>" CRLF;

	while ((d = readdir(dir)) != NULL) {
		std::string	name = d->d_name;
		if (name == ".") {
			continue;
		}
		if (stat(name.c_str(), &buf) == -1) {
			throw HTTPError("RequestHandler::handleAutoIndex ", strerror(errno), 500);
		}
		if (S_ISDIR(buf.st_mode) && name.length() < 50) {
			std::cout << name << " IS A DIRECTORY???" << std::endl;
			name.append(1, '/');
		}
		if (name.length() > 50) {
			name.resize(47);
			name.append("..>");
		}
		body += "<a href=\"";
		body += d->d_name;
		body += "\">" + name + "</a>";
		for (int i = name.length(); i < 51; ++i) {
			body.append(1, ' ');
		}
		timeptr = localtime(&buf.st_mtime);
		strftime(s, 1024, "%d-%b-%Y %R", timeptr);
		body.append(s);
		body.append("\t\t\t");
		if (S_ISDIR(buf.st_mode)) {
			body += '-';
		} else {
			body += intToString(buf.st_size);
		}
		body += CRLF;
	}

	body += "</pre><hr></body>" CRLF;
	body += "</html>" CRLF;

	if (closedir(dir) == -1) {
		throw HTTPError("RequestHandler::handleAutoIndex ", strerror(errno), 500);
	}

	this->m_client->m_response_data.m_content_type = "text/html";

	return body;
}

std::string RequestHandler::Allow() {
	std::string allowed = "Allow: "; // get this resource from allowed methods from the location
	allowed += this->m_client->m_request_data.m_location->second["allow_method"];
	return allowed + CRLF;
}

std::string RequestHandler::Content_Language() {
	return "";
}

std::string RequestHandler::Content_Length(std::string const & body) {
	std::string content_length = "Content-Length: " + intToString(body.length());

	return content_length + CRLF;
}

std::string RequestHandler::Content_Location() {
	return "";
}

std::string RequestHandler::Content_Type() {
	std::string	content_type = "Content-Type: ";

	content_type += this->m_client->m_response_data.m_content_type;

	if (this->m_client->m_response_data.m_content_type.empty()) {
		content_type += "text/plain";
	}

	return content_type + CRLF;
}

std::string RequestHandler::Date() {
	struct timeval	tp;
	std::string	date = "Date: ";

	if (gettimeofday(&tp, NULL) == -1) {
		throw HTTPError("RequestHandler::Date ", strerror(errno), 500);
	}
	date += getDate(&tp.tv_sec);
	return date + CRLF;
}

std::string RequestHandler::Last_Modified() {
	struct stat	buf;
	std::string	last_modified = "Last-Modified: ";

	if (stat(this->m_client->m_request_data.m_real_path.c_str(), &buf) == -1) {
		throw HTTPError("RequestHandler::Last_Modified ", strerror(errno), 500);
	}
	last_modified += getDate(&buf.st_mtime);
	return last_modified + CRLF;
}

std::string RequestHandler::Location() {
	return "";
}

std::string RequestHandler::Retry_After() {
	return "";
}

std::string RequestHandler::Server() {
	std::string	server = "Server: ";
	server += SERVER_VERSION;

	return server + CRLF;
}

std::string RequestHandler::Transfer_Encoding() {
	return "Transfer-Encoding: chunked" CRLF;
}

std::string RequestHandler::WWW_Authenticate() {
	std::string	www_authenticate = "WWW-Authenticate: Basic realm=";
	www_authenticate += this->m_client->m_request_data.m_location->second["auth_basic"]; // get from location
	www_authenticate += ", charset=\"UTF-8\"";

	return www_authenticate + CRLF;
}

std::string RequestHandler::statusLine(int status_code) {
	if (status_code == 0) {
		status_code = this->m_client->m_request_data.m_status_code;
	}

	std::string	status_line;

	status_line.append("HTTP/1.1 ");
	status_line.append(intToString(status_code));
	status_line.append(1, ' ');
	status_line.append(m_status_codes[status_code]);

	return status_line + CRLF;
}

std::string RequestHandler::responseBody() {
	std::string	body;
	int fd = open(this->m_client->m_request_data.m_file.c_str(), O_RDONLY);
	if (fd == -1) {
		throw HTTPError("RequestHandler::responseBody", "error opening file", 500);
	}
	char 	buf[1025];
	ssize_t ret;
	do {
		std::fill_n(buf, 1025, 0);
		ret = read(fd, buf, 1024);
		if (ret == -1) {
			throw HTTPError("RequestHandler::responseBody", "error reading file", 500);
		}
		body.append(buf);
	} while (ret > 0);

	if (close(fd) == -1) {
		throw HTTPError("RequestHandler::responseBody", "error closing file", 500);
	}

	return body;
}

std::string RequestHandler::responseHeaders(std::string const & body) {
	std::string	response_headers;

	if (this->m_client->m_request_data.m_status_code == 405) {
		response_headers += Allow();
	}

	response_headers += Content_Length(body);
	response_headers += Content_Type();
	response_headers += Date();
	response_headers += Last_Modified();
	response_headers += Server();

	if (this->m_client->m_request_data.m_status_code == 401) {
		response_headers += WWW_Authenticate();
	}

	return response_headers;
}

std::string RequestHandler::handleGET() {

	std::string status_line = statusLine();
	std::string response_body;
	if (this->m_client->m_request_data.m_autoindex) {
		response_body = handleAutoIndex();
	} else {
		response_body = responseBody();
	}
	std::string	response_headers = responseHeaders(response_body);

	return status_line + response_headers + CRLF + response_body;
}

std::string RequestHandler::handleHEAD() {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
	std::string	response_headers = responseHeaders(response_body);

	return status_line + response_headers + CRLF;
}

std::string RequestHandler::handlePOST() {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
	std::string	response_headers = responseHeaders(response_body);

	return status_line + response_headers + CRLF + response_body;
}

std::string RequestHandler::handlePUT() {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
	std::string	response_headers = responseHeaders(response_body);

	return status_line + response_headers + CRLF + response_body;
}

std::string RequestHandler::handleDELETE() {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
	std::string	response_headers = responseHeaders(response_body);

	return status_line + response_headers + CRLF + response_body;
}

std::string	RequestHandler::generateErrorPage(int error) {
	std::string status_line = statusLine();

	std::string	error_response =
			"<html>" CRLF
			"<head><title>" + intToString(error) + ' ' + m_status_codes[error] + "</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>" + intToString(error) + ' ' + m_status_codes[error] + "</h1></center>" CRLF
			"<hr><center>" + SERVER_VERSION + "</center>" CRLF
			"</body>" CRLF
			"</html>" CRLF
			;

	this->m_client->m_response_data.m_content_type = "text/html";
	std::string	response_headers = responseHeaders(error_response);

	return status_line + response_headers + CRLF + error_response;
}

void	RequestHandler::handleMetadata(t_client &c) {
	if (c.m_request_data.m_error)
		return ; //don't need to do anything if an error has been detected in RequestParser
	std::cout<<"handling metadata.."<<std::endl;

	this->m_client = &c;

	try {
		//updating virtual server pointer based on client request's host header
		m_client->updateServerConf();
		//updating location block
		c.m_request_data.m_location = c.m_v_server->getLocation(c.m_request_data);
		c.m_request_data.m_owner = &c;
		std::cout<<"-------FETCHED BLOCK-------\n\tLISTEN "
			<<c.m_v_server->m_configs.m_directives["listen"]
			<<"\n\tSERVER_NAME "<< m_client->m_v_server->m_configs.m_directives["server_name"]
			<<"\n\tLOCATION/ROUTE "<< m_client->m_request_data.m_location->first<<"\n-----------"<<std::endl;
		std::string &real_path =  c.m_request_data.m_real_path;
		std::string	stat_file;
		real_path = c.m_request_data.m_path;
		std::string const & location = c.m_request_data.m_location->first;
		std::string & alias = c.m_request_data.m_location->second["alias"];
		/*replacing location path by alias path (what if alias empty?)*/
		if (alias[alias.size() - 1] != '/')
			alias.append("/");
		real_path.replace(real_path.find(location), location.length(), alias);
		std::cout<<"real_path: "<<real_path<<std::endl;
		size_t	prefix = 0;
		size_t	next_prefix = 0;
		//stat every /prefix/ until
		// 						found file
		// 						end of URI
		// 						stat returns -1, so throw error not found
		while (prefix < real_path.size()) {
			prefix = real_path.find_first_of("/?", prefix);
			next_prefix = prefix == std::string::npos ? std::string::npos : real_path.find_first_of("?/", prefix + 1);
			stat_file = real_path.substr(0, next_prefix);
			std::cout<<"\tstat "<<stat_file<<std::endl;
			if (stat(stat_file.c_str(), &this->m_statbuf)) {
				throw HTTPError("RequestHandler::handleMetadata", "invalid full path", 404);
			}
			//also check permission
			if ((this->m_statbuf.st_mode & S_IFMT) == S_IFREG) // if file
				break ;
			if ((this->m_statbuf.st_mode & S_IFMT) != S_IFDIR
					&& (this->m_statbuf.st_mode & S_IFMT) != S_IFLNK) { // if something else than a directory/smlink
				throw HTTPError("RequestHandler::handleMetadata", "invalid full path, not a file/directory/symlink", 404);
			}
			if (next_prefix == std::string::npos)
				break ;
			prefix = next_prefix;
		}
			c.m_request_data.m_file = real_path.substr(prefix + 1, std::string::npos);

		// if we stopped at file
		if ((this->m_statbuf.st_mode & S_IFMT) == S_IFREG) {
			std::cout<<"m_real_path: "<<c.m_request_data.m_real_path<<std::endl;
			std::cout<<"m_file: "<<c.m_request_data.m_file<<std::endl;
			std::cout<<"stat file: "<<stat_file<<std::endl;
			// 	extract extension
			size_t	extension = c.m_request_data.m_file.find_last_of('.', c.m_request_data.m_file.size());
			std::cout<<"extension index: "<<extension<<std::endl;
			// 	if CGI directives exist && extension == cgi directive && cgi_path is valid
			if (this->validCgi(c.m_request_data, extension))
			{
				std::cout<<"cgi detected"<<std::endl;
				this->handleCgiMetadata(c.m_request_data, stat_file);
			}
			else
			{
				//	normal file;
				//	check extension against mime types;
				this->m_client->m_response_data.m_content_type = this->m_mime_types[stat_file.substr(stat_file.rfind('.') + 1)];
				std::cout << "content-type: "<<this->m_client->m_response_data.m_content_type<<std::endl;
				//	if there are additional entries after this file, we throw bad request
				if (next_prefix != std::string::npos) {
					throw HTTPError("RequestHandler::handleMetadata", "invalid full path", 404);
				}
			}
		} 
		//		see if autoindex on then flag it so handleRequest can call the appropriate method
		//		else return bad request?
		//		//we dont need to check if remaining entries, because loop on 141 doesn't stop at a directory unless it's the last prefix in uri
		else if (c.m_request_data.m_location->second["autoindex"] == "on")
		{
			std::cout<<"dir listing"<<std::endl;
			c.m_request_data.m_autoindex= true;
		}
		else
			throw HTTPError("RequestHandler::handleMetadata", "directory listing not enabled", 403);

		AllowedMethods(c);
		Authenticated(c);
	} catch (HTTPError & e) {
		std::cerr << e.what() << std::endl;
		m_client->m_request_data.m_error = true;
		m_client->m_request_data.m_status_code = e.HTTPStatusCode();
		m_client->m_request_data.m_done = true;
	}
}

void	RequestHandler::handleRequest(t_client &c) {
	this->m_client = &c;
	Request	&request = this->m_client->m_request_data;

	try {
		if (request.m_error) {
			this->m_client->m_response_str = generateErrorPage(request.m_status_code);
		} else if (c.m_request_data.m_cgi) {
			this->m_cgi.run(c);
		} else {
			switch (request.m_method) {
				case GET:
					m_client->m_response_str = handleGET();
					break;
				case HEAD:
					m_client->m_response_str = handleHEAD();
					break;
				case POST:
					m_client->m_response_str = handlePOST();
					break;
				case PUT:
					m_client->m_response_str = handlePUT();
					break;
				case DELETE:
					m_client->m_response_str = handleDELETE();
					break;
			}
		}
	} catch (HTTPError & e) {
		std::cerr << e.what() << std::endl;
		this->m_client->m_request_data.m_error = true;
		this->m_client->m_request_data.m_status_code = e.HTTPStatusCode();
		this->m_client->m_response_str = generateErrorPage(this->m_client->m_request_data.m_status_code);
	}
}
