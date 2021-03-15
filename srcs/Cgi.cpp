#include "Cgi.hpp"
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "Error.hpp"
#include "utils.hpp"
#include "WebServer.hpp"
#include "RequestParser.hpp"

Cgi::Cgi()
	: m_env_array(0), m_argv(0){}

Cgi::~Cgi() {
	this->clear();
}

//only allocates memory when Cgi is used
void	Cgi::init() {
	if (this->m_env_array)
		return ;
	this->m_env_array = static_cast<char **>(malloc(sizeof(char *) * CGI_ENV_LEN));
	if (!this->m_env_array)
		throw (serverError("Cgi::init", "failed to allocate cgi env"));
	if (this->m_argv)
		return ;
	this->m_argv = static_cast<char **>(malloc(sizeof(char *) * 3));
}

void	Cgi::convertEnv(t_client &c) {
	t_cgi_env_map::iterator it = this->m_env_map.begin();
	size_t	i = 0;
	while (it != this->m_env_map.end()) {
		std::string	convert(it->first + "=" + it->second);
		this->m_env_array[i] = ft_strdup(convert);
		if (!this->m_env_array[i])
			throw (serverError("Cgi::convertEnv", "failed to allocate cgi env"));
		std::cout<<this->m_env_array[i]<<std::endl;//remove
		++it;
		++i;
	}
	this->m_env_array[i] = 0;
	this->m_argv[0] = ft_strdup(c.m_request_data.m_location->second["cgi_path"]);
	this->m_argv[1] = ft_strdup(c.m_request_data.m_file);
	this->m_argv[2] = 0;
}

void	Cgi::read(t_client &c) {
	std::cout<<"cgi read"<<std::endl;
	(void)c;
}

void	Cgi::write(t_client &c) {
	std::cout<<"cgi write"<<std::endl;
	(void)c;
}

void	Cgi::exec(t_client &c) {
	(void)c;
	std::cout<<"exec with "<<this->m_argv[0] << " "<<this->m_argv[1]<<std::endl;
	if ((c.m_cgi_pid = fork()) == -1) {
		this->clear();
		throw HTTPError("Cgi::exec: fork", strerror(errno), 500); //do we stop server after we throw
	}
	if (!c.m_cgi_pid) {
		if (execve(this->m_argv[0], this->m_argv, this->m_env_array) == -1) {
			this->clear();
			throw HTTPError("Cgi::exec: execve", strerror(errno), 500);
		}
	}
	int	wstatus = 0;
	if (waitpid(c.m_cgi_pid, &wstatus, WNOHANG) == -1) {
			this->clear();
			throw HTTPError("Cgi::exec: waitpid", strerror(errno), 500);
	}
}

void	Cgi::run(t_client &c) {
	std::cout<<"run cgi"<<std::endl;
	if (!c.m_cgi_running) {
		c.m_cgi_running = true;
		this->init();
		this->fillEnv(c.m_request_data);
		this->convertEnv(c);
		if (c.m_request_data.m_method == POST) {
			c.m_cgi_write = c.m_request_data.m_body.size();
		}
		this->exec(c);
		c.m_cgi_running = false;
	}
	if (c.m_cgi_write > 0) {
		this->write(c);
	}
	this->read(c);
}

/*TODO*/
/*
 * recode inet_ntoa
 * */

void	Cgi::fillEnv(t_request_data &request) {
	char	buf[PATH_MAX];
	(void)buf;
	this->m_env_map["AUTH_TYPE"]=std::string(request.m_headers[AUTHORIZATION]);
	this->m_env_map["CONTENT_LENGTH"]=intToString(request.m_body.size());
	this->m_env_map["CONTENT_TYPE"]=std::string(request.m_headers[CONTENTTYPE]);
	this->m_env_map["GATEWAY_INTERFACE"]="CGI/1.1";
	this->m_env_map["PATH_INFO"]=request.m_path_info; //  used to specify the name of the file to be opened and interpreted  by the cgi program
	this->m_env_map["PATH_TRANSLATED"]= getcwd(buf, PATH_MAX) + this->m_env_map["PATH_INFO"]; //or htdocs
	this->m_env_map["QUERY_STRING"] = request.m_query_string;
	struct	sockaddr_in	*tmp = reinterpret_cast<struct sockaddr_in*>(&request.m_owner->m_sockaddr);
	this->m_env_map["REMOTE_ADDR"] = inet_ntoa(tmp->sin_addr);//replace
	this->m_env_map["REMOTE_IDENT"] =""; //?
	this->m_env_map["REMOTE_USER"] =""; //?
	this->m_env_map["REQUEST_METHOD"] = methods[request.m_method]; //maybe simpler way?
	this->m_env_map["REQUEST_URI"] = request.m_path;
	this->m_env_map["SCRIPT_FILENAME"] = request.m_file; //if cgi-bin/test.php script_name is cgi-bin/test-cgi.php
	this->m_env_map["SERVER_NAME"] ="webserv";
	this->m_env_map["SERVER_PORT"] = request.m_owner->m_v_server->m_port;
	this->m_env_map["SERVER_PROTOCOL"]="HTTP/1.1";
	this->m_env_map["SERVER_SOFTWARE"]="HTTP 1.1";
	this->m_env_map["REDIRECT_STATUS"]="true"; // see if need to be disabled
}

void	Cgi::reset() {
	if (this->m_env_array) {
		for (size_t	i = 0; this->m_env_array[i]; ++i) {
			if (this->m_env_array[i])
				free(this->m_env_array[i]);
			this->m_env_array[i] = 0;
		}
	}
	if (this->m_argv)
	{
		if (this->m_argv[0])
			free(this->m_argv[0]);
		if (this->m_argv[1])
			free(this->m_argv[1]);
		this->m_argv[0] = 0;
		this->m_argv[1] = 0;
	}
}

void	Cgi::clear() {
	this->reset();
	if (this->m_env_array)
		free(this->m_env_array);
	if (this->m_argv)
		free(this->m_argv);
	this->m_env_array = 0;
	this->m_argv = 0;
}

void	RequestHandler::handleCgiMetadata(t_request &request, std::string &file) {
	request.m_cgi = true;
	if (request.m_real_path.size() - file.size() == 0)
		return ;
	size_t	query_string_index = request.m_file.find('?', 0);
	if (query_string_index != std::string::npos)
		request.m_query_string = request.m_file.substr(query_string_index + 1, std::string::npos);
	size_t	path_info_index = request.m_file.find('/', 0);
	if (path_info_index != std::string::npos) 
		request.m_path_info = request.m_file.substr(path_info_index, query_string_index - path_info_index);
	request.m_file = file;
	//checek permission for path_info
	std::cout<<"file: "<<request.m_file<<std::endl;
	std::cout<<"path_info: "<<request.m_path_info<<std::endl;
	std::cout<<"query_string : "<<request.m_query_string<<std::endl;
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
		if (file.compare(extension_index, extension_found->second.size(), extension_found->second))
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
		return true;
	}
	else if (path_found != request.m_location->second.end()) {
		std::cout<<"handleCgiMetadata: no cgi extension provided"<<std::endl;
	}
	return false;
}
