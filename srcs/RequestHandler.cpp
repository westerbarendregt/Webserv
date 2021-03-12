#include <iostream>
#include "Error.hpp"
#include "RequestHandler.hpp"
#include "WebServer.hpp"
#include "Server.hpp"
#include "utils.hpp"

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

	return status_line;
}

std::string RequestHandler::responseBody() {
	return "";
}

std::string RequestHandler::responseHeaders(std::string const & body) {

	m_response_headers.push_back(Server());
	m_response_headers.push_back(Content_Length(body));
	m_response_headers.push_back(Content_Type());


	std::string	response_headers;

	std::vector<std::string>::iterator it = m_response_headers.begin();
	for (; it != m_response_headers.end(); ++it) {
		response_headers.append(*it);
	}
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

	std::string	error_response =
			"<html>" CRLF
			"<head><title>" + intToString(error) + ' ' + m_status_codes[error] + "</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>" + intToString(error) + ' ' + m_status_codes[error] + "</h1></center>" CRLF
			;

	std::string	response_headers = responseHeaders(error_response);

	return status_line + response_headers + CRLF + error_response;
}

void	RequestHandler::handleMetadata(t_client &c) {
	if (c.m_request_data.m_error)
		return ; //don't need to do anything if an error has been detected in RequestParser
	std::cout<<"handling metadata.."<<std::endl;

	m_client = &c;

	try {
		//updating virtual server pointer based on client request's host header
		m_client->updateServerConf();
		c.m_request_data.m_location = c.m_v_server->getLocation(c.m_request_data);
		std::cout<<"-------FETCHED BLOCK-------\n\tLISTEN "
			<<c.m_v_server->m_configs.m_directives["listen"]
			<<"\n\tSERVER_NAME "<< m_client->m_v_server->m_configs.m_directives["server_name"]
			<<"\n\tLOCATION/ROUTE "<< m_client->m_request_data.m_location->first<<"\n-----------"<<std::endl;
		//c.m_request_data.m_real_path =  substr
		//maybe Request could have m_real_path, m_path_info and m_query_string to be updated by the cgi part of this code,
		//	and later fetched by Cgi to populate the map
		// select the route 
		// subsitute route + rest of URI and update m_real_path
		// stat every /prefix/ until
		// 						found file
		// 						end of URI
		// 						stat returns -1, so throw error not found
		// if we stopped at file
		// 		extract extension
		// 		if CGI directives exist && extension == cgi directive && cgi_path is valid
		// 			we can mark the request as CGI and update m_path_info, m_query_string
		// 			different function
		//		else 
		//			check extension against mime types;
		//			if there are additional entries after this file, we throw bad request
		//else if directory
		//		see if autoindex on then flag it so handleRequest can call the appropriate method
		//		else return bad request?
		//		//we dont need to check if remaining entries, because loop on 141 doesn't stop at a directory unless it's the last prefix in uri
		//

		// Authorization / WWW-Authenticate
		// Allow
		//
	} catch (HTTPError & e) {
		std::cerr << e.what() << std::endl;
		m_client->m_request_data.m_error = e.HTTPStatusCode();
		m_client->m_request_data.m_done = true;
	}
}

//Incomplete cgi parsing
void	RequestHandler::handleCgiMetadata(t_client &c) {
	t_routes	&routes = c.m_v_server->m_configs.m_routes;
	t_routes::iterator root; //temporary
	if ((root = routes.find("/")) == routes.end())
	{
		std::cout<<"no root found"<<std::endl;
		return ;
	}
	t_directives::iterator	path_found = root->second.find("cgi_path");
	t_directives::iterator	extension_found = root->second.find("cgi");
	std::string	&uri = c.m_request_data.m_path;

	if (extension_found != root->second.end()) {
		//compare  with uri
		if (uri.compare(uri.size() - extension_found->second.size(), extension_found->second.size(), extension_found->second))
			return ; //uri is not to be treated as cgi (incomplete detection, see PATH_INFO)
		if (path_found == root->second.end()) {
			std::cout<<"handleCgiMetadata: no path to cgi exec provided"<<std::endl;
			return ;
		}
		//check for existence of path here
		if (stat(path_found->second.c_str(), &this->m_statbuf)) {
			std::cout<<"handleCgiMetadata: invalid path to cgi executable"<<std::endl;
		}
		c.m_request_data.m_cgi = true;
	}
	else if (path_found != root->second.end()) {
		std::cout<<"handleCgiMetadata: no cgi extension provided"<<std::endl;
		return ;
	}
}

void	RequestHandler::handleRequest(t_client &c) {
	m_client = &c;
	Request	&request = m_client->m_request_data;

	if (request.m_error != 0) {
		m_client->m_response_str = generateErrorPage(request.m_error);
	} else if (false) {
		//
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
