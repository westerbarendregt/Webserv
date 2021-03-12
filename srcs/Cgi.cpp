#include "Cgi.hpp"
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "Error.hpp"
#include "utils.hpp"
#include "WebServer.hpp"

Cgi::Cgi()
	: m_av(0) {}

Cgi::~Cgi() {
	if (this->m_av) {
		free(this->m_av);
	}
}

//only allocates memory when Cgi is used
void	Cgi::init() {
	if (this->m_av)
		return ;
	this->m_av = reinterpret_cast<char **>(malloc(sizeof(char *) * CGI_ENV_LEN));
	if (!this->m_av)
		throw (serverError("Cgi::init", "failed to allocate cgi env"));
}

void	Cgi::convertEnv() {
	t_cgi_env::iterator it = this->m_env.begin();
	size_t	i = 0;
	while (it != this->m_env.end()) {
		std::string	convert = it->first + "=" + it->second;
		this->m_av[i] = ft_join(convert.c_str(), 0, convert.size() - 1);
		if (!this->m_av[i])
			throw (serverError("Cgi::convertEnv", "failed to allocate cgi env"));
		++it;
		++i;
		std::cout<<this->m_av[i]<<std::endl;//remove
	}
}

void	Cgi::run(t_client &c) {
	std::cout<<"run cgi"<<std::endl;
	c.m_response_str = "HTTP/1.1 200 OK\r\nDate: Mon, 27 Jul 2009 12:28:53 GMT\r\nServer: webserv\r\nLast-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\nContent-Length: 5\r\nVary: Accept-Encoding\r\nContent-Type: text/plain\r\n\r\nCgi\r\n";
	return ; //for now
	if (!this->m_av)
		this->init();
	this->fillEnv(c.m_request_data);
	this->convertEnv();
}

void	Cgi::fillEnv(t_request_data &request) {
	char	buf[PATH_MAX];
	this->m_env["AUTH_TYPE"]=request.m_headers[AUTHENTIZATION];
	this->m_env["CONTENT_LENGTH"]=sputnbr(request.m_body.size());
	this->m_env["CONTENT_TYPE"]=request.m_headers[CONTENTTYPE];
	this->m_env["GATEWAY_INTERFACE"]="CGI/1.1";
	this->m_env["PATH_INFO"]=request.m_path_info; //
	this->m_env["PATH_TRANSLATED"]= getcwd(buf, PATH_MAX) + this->m_env["PATH_INFO"];
	this->m_env["QUERY_STRING"]=request.m_query_string;
	this->m_env["REMOTE_ADDR"]= "";
	this->m_env["REMOTE_IDENT"]="";
	this->m_env["REMOTE_USER"]="";
	this->m_env["REQUEST_METHOD"]="GET";
	this->m_env["REQUEST_URI"]="/test-cgi.php";
	this->m_env["SCRIPT_NAME"]="test-cgi.php";
	this->m_env["SERVER_NAME"]="webserv";
	this->m_env["SERVER_PORT"]= request.m_owner->m_v_server->m_port;
	this->m_env["SERVER_PROTOCOL"]="HTTP /1.1";
	this->m_env["SERVER_SOFTWARE"]="HTTP 1.1";
}
//Incomplete cgi parsing
void	RequestHandler::handleCgiMetadata(t_request &request) {
	if (request.m_path.size() == request.m_file.size()) {
		return ;
	}
	std::string rhs = request.m_path.substr(request.m_file.size(), std::string::npos);
	size_t	
	//get query string
	//get path_info
	(void)request;
}

bool	RequestHandler::validCgi(t_request &request, size_t extension_index) {
	if (extension_index == std::string::npos)
		return false;
	t_directives::iterator	path_found = request.m_location->second.find("cgi_path");
	t_directives::iterator	extension_found = request.m_location->second.find("cgi");
	std::string	&file = request.m_file;
//
	if (extension_found != request.m_location->second.end()) {
		//compare  with uri
		if (file.compare(extension_index, file.size() - extension_index, extension_found->second))
			return false; //uri is not to be treated as cgi
		if (path_found == request.m_location->second.end()) {
			std::cout<<"handleCgiMetadata: no path to cgi exec provided"<<std::endl;
			return false;
		}
		//check for existence of path here
		if (stat(path_found->second.c_str(), &this->m_statbuf)) {
			std::cout<<"handleCgiMetadata: invalid path to cgi executable"<<std::endl;
			return false;
		}
		request.m_cgi = true;
	}
	else if (path_found != request.m_location->second.end()) {
		std::cout<<"handleCgiMetadata: no cgi extension provided"<<std::endl;
		return false;
	}
	return true;
}
