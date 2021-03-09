#include <iostream>
#include "RequestHandler.hpp"

#define CRLF "\r\n"

RequestHandler::RequestHandler() {
	initStatusCodes();
	initMimeTypes();
}

RequestHandler::~RequestHandler() {}

std::string RequestHandler::Content_Type() {
	std::string content_type;

	return content_type;
}

std::string RequestHandler::statusLine() {
	//
}

std::string RequestHandler::responseBody() {
	//
}

std::string RequestHandler::responseHeaders() {

	m_response_headers.push_back(Content_Type());
}

std::string RequestHandler::handleGET() {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
}

std::string RequestHandler::handleHEAD() {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
}

std::string RequestHandler::handlePOST() {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
}

std::string RequestHandler::handlePUT() {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
}

std::string RequestHandler::handleDELETE() {
	std::string status_line = statusLine();
	std::string response_body = responseBody();
}

std::string	RequestHandler::generateErrorPage(int error) {
	std::string	error_response;

	error_response =
			"<html>" CRLF
			"<head><title>" + itoa(error) + ' ' + m_status_codes[error] + "</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>" + itoa(error) + ' ' + m_status_codes[error] + "</h1></center>" CRLF
			;
}

void	RequestHandler::handleMetadata(t_client &c) {
	c.m_request_data.m_done = true;
}

void	RequestHandler::handleRequest(t_client &c) {
	Request	&request = c.m_request_data;

	if (request.m_error != 0) {
		c.m_response_str = generateErrorPage(request.m_error);
	} else if (/* cgi */) {
		//
	} else {
		switch (request.m_method) {
			case GET:
				c.m_response_str = handleGET();
				break;
			case HEAD:
				c.m_response_str = handleHEAD();
				break;
			case POST:
				c.m_response_str = handlePOST();
				break;
			case PUT:
				c.m_response_str = handlePUT();
				break;
			case DELETE:
				c.m_response_str = handleDELETE();
				break;
		}
	}
}
