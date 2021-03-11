#include <iostream>
#include "RequestHandler.hpp"
#include "WebServer.hpp"
#include "Server.hpp"
#include "utils.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define CRLF "\r\n"

RequestHandler::RequestHandler() {
	initStatusCodes();
	initMimeTypes();
}

RequestHandler::~RequestHandler() {}

std::string RequestHandler::Content_Length(std::string const & body) {
	std::string content_length = "Content-Length: " + intToString(body.length());

	return content_length;
}

std::string RequestHandler::Content_Type() {
	std::string content_type;

	return content_type;
}

std::string RequestHandler::Server() {
	std::string server = "webserv/1.0.0" CRLF;

	return server;
}

std::string RequestHandler::statusLine(t_client &c) {
	int error_code = c.m_request_data.m_error;
	std::string	status_line;

	status_line.append("HTTP/1.1 ");
	status_line.append(intToString(error_code));
	status_line.append(' ');
	status_line.append(m_status_codes[error_code]);

	return status_line;
}

std::string RequestHandler::responseBody() {
	return "";
}

std::string RequestHandler::responseHeaders(t_client &c, std::string const & body) {

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

std::string RequestHandler::handleGET(t_client &c) {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
	std::string	response_headers = responseHeaders(response_body);

	return status_line + response_headers + CRLF + response_body;
}

std::string RequestHandler::handleHEAD(t_client &c) {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
	std::string	response_headers = responseHeaders(response_body);

	return status_line + response_headers + CRLF;
}

std::string RequestHandler::handlePOST(t_client &c) {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
	std::string	response_headers = responseHeaders(response_body);

	return status_line + response_headers + CRLF + response_body;
}

std::string RequestHandler::handlePUT(t_client &c) {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
	std::string	response_headers = responseHeaders(response_body);

	return status_line + response_headers + CRLF + response_body;
}

std::string RequestHandler::handleDELETE(t_client &c) {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
	std::string	response_headers = responseHeaders(response_body);

	return status_line + response_headers + CRLF + response_body;
}

std::string	RequestHandler::generateErrorPage(int error) {
	std::string	error_response;

	error_response =
			"<html>" CRLF
			"<head><title>" + intToString(error) + ' ' + m_status_codes[error] + "</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>" + intToString(error) + ' ' + m_status_codes[error] + "</h1></center>" CRLF
			;

	return error_response;
}

void	RequestHandler::handleMetadata(t_client &c) {
	c.m_request_data.m_done = true;
	std::cout<<"handling metadata.."<<std::endl;
	// http 1.1 request without a host header should return error 400
	if (c.m_request_data.m_headers[HOST].empty()) {
		c.m_request_data.m_error = 400;
		c.m_request_data.m_done = true;
		return ;
	}
	//updating virtual server pointer based on client request's host header
	c.updateServerConf();
	std::cout<<"-------FETCHED BLOCK-------\n\tLISTEN "<<c.m_v_server->m_configs.m_directives["listen"]<<"\n\tSERVER_NAME "<<
		c.m_v_server->m_configs.m_directives["server_name"]<<"\n--------------"<<std::endl;
}


void	RequestHandler::handleRequest(t_client &c) {
	Request	&request = c.m_request_data;

	if (request.m_error != 0) {
		c.m_response_str = generateErrorPage(request.m_error);
	} else if (false) {
		//
	} else {
		switch (request.m_method) {
			case GET:
				c.m_response_str = handleGET(c);
				break;
			case HEAD:
				c.m_response_str = handleHEAD(c);
				break;
			case POST:
				c.m_response_str = handlePOST(c);
				break;
			case PUT:
				c.m_response_str = handlePUT(c);
				break;
			case DELETE:
				c.m_response_str = handleDELETE(c);
				break;
		}
	}
}
