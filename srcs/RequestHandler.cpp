#include "RequestHandler.hpp"
#include "WebServer.hpp"
#include "Server.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void	RequestHandler::handleMetadata(t_client &c) {
	if (c.m_request_data.m_error)
		return ; //don't need to do anything if an error has been detected in RequestParser
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
	// select appropriate route based on URI, maybe update a pointer to it
	//detect cgi right away because parsing Uri is too different than parsing CGi uri
	this->handleCgiMetadata(c);
	//	if cgi detected, return , else
	//	check extension against mime types
	//	check if ended by slash and if directory listing is on
	//	finally stat file
	//	idea is to provide enough formation so HandleRequest can generate an appropriate response.
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
 // either get resource or execute cgi, both populating the response and adding client_socket to write_all
 //
	std::cout<<"handling request.."<<std::endl;
	if (c.m_request_data.m_cgi) {
		this->m_cgi.run(c);
	}
	c.m_response_str = "HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: webserv\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nContent-Length: 4\r\nVary: Accept-Encoding\r\nContent-Type: text/plain\r\n\r\nOi\r\n";
}
