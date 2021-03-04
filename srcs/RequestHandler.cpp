#include "RequestHandler.hpp"

void	RequestHandler::handleMetadata(t_client &c) {
	std::cout<<"handling metadata.."<<std::endl;
	c.m_request_data.m_done = true;
}

void	RequestHandler::handleRequest(t_client &c) {
 // either get resource or execute cgi, both populating the response and adding client_socket to write_all
 //
	std::cout<<"handling request.."<<std::endl;
	std::cout<<"generating response.."<<std::endl;
	c.m_response_str = "HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: webserv\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nContent-Length: 4\r\nVary: Accept-Encoding\r\nContent-Type: text/plain\r\n\r\nOi\r\n";
 
}
