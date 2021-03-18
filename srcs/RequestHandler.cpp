#include <iostream>
#include "Conf.hpp"
#include "Error.hpp"
#include "RequestHandler.hpp"
#include "WebServer.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "Authentication.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

RequestHandler::RequestHandler() {
	initStatusCodes();
	initMimeTypes();
}

RequestHandler::~RequestHandler() {}

std::string RequestHandler::Content_Length(std::string const & body) {
	std::string content_length = "Content-Length: " + intToString(body.length());

	return content_length + CRLF;
}

std::string RequestHandler::Content_Type() {
	std::string content_type;

	return content_type + CRLF;
}

std::string RequestHandler::Server() {
	std::string server = "webserv/1.0.0";

	return server + CRLF;
}

std::string RequestHandler::statusLine() {
	int error_code = m_client->m_request_data.m_error;
	std::string	status_line;

	status_line.append("HTTP/1.1 ");
	status_line.append(intToString(error_code));
	status_line.append(" ");
	status_line.append(m_status_codes[error_code]);
	status_line.append(CRLF);
	return status_line;
}

std::string RequestHandler::responseBody() {
	return "";
}

std::string RequestHandler::responseHeaders(std::string const & body) {
	m_response_headers.clear();
	m_response_headers.push_back(Server());
	m_response_headers.push_back(Content_Length(body));
	m_response_headers.push_back(Content_Type());


	std::string	response_headers;

	std::vector<std::string>::iterator it = m_response_headers.begin();
	for (; it != m_response_headers.end(); ++it) {
		response_headers.append(*it);
	}

	// response_headers.append(CRLF);
	return response_headers;
}

std::string RequestHandler::handleGET() {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
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
	std::string response_headers;
	if (error == 401)
	{
		response_headers +=	"Server: Webserv/1.1\r\n"
					  	"Content-Type: text/html\r\n"
	   					"WWW-Authenticate: Basic realm=";
		response_headers += this->m_client->m_request_data.m_location->second["auth_basic"]; // get from location
		response_headers += ", charset=\"UTF-8\"\r\n";
		return status_line + response_headers + CRLF;
	}
	if (error == 405)
	{
    	std::string allowed = this->m_client->getRequest().m_location->second["allow_method"]; // get this resource from allowed methods from the location
		response_headers +=	"Server: Webserv/1.1\r\n"
					  	"Content-Type: text/html\r\n"
	   					"Allow: ";
		response_headers += allowed;
		response_headers += CRLF;
		return status_line + response_headers + CRLF;
	}

	std::string	error_response =
			"<html>" CRLF
			"<head><title>" + intToString(error) + ' ' + m_status_codes[error] + "</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>" + intToString(error) + ' ' + m_status_codes[error] + "</h1></center>" CRLF
			;

	response_headers = responseHeaders(error_response);

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
		std::string const & alias = c.m_request_data.m_location->second["alias"];
		/*replacing location path by alias path (what if alias empty?)*/
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
			throw HTTPError("RequestHandler::handleMetadata", "directory listing not enabled", 404);
		AllowedMethods(c);
		Authenticated(c);
	} catch (HTTPError & e) {
		std::cerr << e.what() << std::endl;
		m_client->m_request_data.m_error = e.HTTPStatusCode();
		m_client->m_request_data.m_done = true;
	}
}

// std::string	handlePut()
// {
// 	std::string status_line;
// 	std::string response_headers;

// 	std::string path_to_file = m_request_data->

// 	return status_line + response_headers + CRLF;
// }

void	RequestHandler::handleRequest(t_client &c) {
	this->m_client = &c;
	this->m_request_data = &c.m_request_data;
	Request	&request = m_client->m_request_data;
	if (request.m_error != 0) {
		m_client->m_response_str = generateErrorPage(request.m_error);
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
}
