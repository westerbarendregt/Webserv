#include <iostream>
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
	// status_line.append(CRLF);
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

	return status_line + CRLF + response_headers + CRLF + response_body;
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
	std::string response;
	if (error == 401)
	{
		response +=	"Server: Webserv/1.1\r\n"
					  	"Content-Type: text/html\r\n"
	   					"WWW-Authenticate: Basic realm=";
		response += "\"Access to the production webserv\"";
		response += ", charset=\"UTF-8\"\r\n";
		return status_line + CRLF + response + CRLF;
	}
	if (error == 405)
	{
    	std::string allowed = "GET, POST, HEAD\n"; // get this resource from allowed methods from the location
		response +=	"Server: Webserv/1.1\r\n"
					  	"Content-Type: text/html\r\n"
	   					"Allow: ";
		response += allowed;
		return status_line + CRLF + response + CRLF;
	}

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
	std::cout<<"handling metadata.."<<std::endl;

	m_client = &c;

	try {
		//updating virtual server pointer based on client request's host header
		m_client->updateServerConf();
		std::cout<<"-------FETCHED BLOCK-------\n\tLISTEN "<<c.m_v_server->m_configs.m_directives["listen"]<<"\n\tSERVER_NAME "<<
				 m_client->m_v_server->m_configs.m_directives["server_name"]<<"\n--------------"<<std::endl;
		m_client->m_request_data.m_location = getLocation();
		// getRealPath();
		/*
		 * format full path to file
		 * check extension (locate first dot)
		 *
		 * if CGI directives exist && ext == cgi directive
		 * 		mark as CGI
		 * 		separate query string (if there is one)
		 */
		// if (cgi) {
		// 		checkCGIPath(); // check for existence of CGI executable in cgi_path
		// }
		// checkRealPath(); // checks if real path is a file or a directory and flags if autoindex is enabled
		AllowedMethods(c);
		Authenticated(c);
		// Authorization / WWW-Authenticate
		// Allow
		//
	} catch (HTTPError & e) {
		std::cerr << e.what() << std::endl;
		m_client->m_request_data.m_error = e.HTTPStatusCode();
		m_client->m_request_data.m_done = true;
	}
}

void	RequestHandler::handleRequest(t_client &c) {
	m_client = &c;
	Request	&request = m_client->m_request_data;

	std::cout << "HIER!!! " << request.m_error << std::endl;
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
