#include "Cgi.hpp"
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include "Error.hpp"
#include "utils.hpp"
#include "WebServer.hpp"
#include "RequestParser.hpp"

Cgi::Cgi()
	: m_env_array(0), m_argv(0)
{
}

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
		this->m_env_array[i] = ft::strdup(convert);
		if (!this->m_env_array[i])
			throw (serverError("Cgi::convertEnv", "failed to allocate cgi env"));
		std::cout<<this->m_env_array[i]<<std::endl;//remove
		++it;
		++i;
	}
	this->m_env_array[i] = 0;
	this->m_argv[0] = ft::strdup(c.m_request_data.m_location->second["cgi_path"]);
	this->m_argv[1] = ft::strdup(c.m_request_data.m_file);
	this->m_argv[2] = 0;
}

void	Cgi::read(t_client &c) {
	char buf[5000];

	std::fill(buf, buf + sizeof(buf), 0);
	ssize_t	nbytes = ::read(c.getReadFd(), buf, 4096);
	if (nbytes == -1) {
		throw HTTPError("Cgi::read", strerror(errno), 500);
	}
	if (nbytes)
		c.m_cgi_out_buf.append(buf);
}


void	Cgi::stop(t_client &c) {
	if (c.m_cgi_read_pipe[IN] != -1 && ::close(c.m_cgi_read_pipe[IN]) == -1) {
		std::cout << "Cgi::stop : close(m_cgi_read_pipe[IN]): " << strerror(errno)<<std::endl;
	}
	if (c.m_cgi_read_pipe[OUT] != -1 && ::close(c.m_cgi_read_pipe[OUT]) == -1) {
		std::cout << "Cgi::stop : close(m_cgi_read_pipe[OUT]): " << strerror(errno)<<std::endl;
	}
	if (c.m_cgi_write_pipe[IN] != -1 && ::close(c.m_cgi_write_pipe[IN]) == -1) {
		std::cout << "Cgi::stop : close(m_cgi_write_pipe[IN]): " << strerror(errno)<<std::endl;
	}
	if (c.m_cgi_write_pipe[OUT] != -1 && ::close(c.m_cgi_write_pipe[OUT]) == -1) {
		std::cout << "Cgi::stop : close(m_cgi_write_pipe[OUT]): " << strerror(errno)<<std::endl;
	}
	c.m_cgi_read_pipe[IN] =-1;
	c.m_cgi_read_pipe[OUT] =-1;
	c.m_cgi_write_pipe[IN] =-1;
	c.m_cgi_write_pipe[OUT] =-1;
	c.m_cgi_running = false;
	c.m_range_fd = 0;
}

void	Cgi::write(t_client &c) {
	std::cout<<"write"<<std::endl;
	size_t	len = c.m_request_data.m_body.size() + 1 - c.m_cgi_write_offset;
	ssize_t	nbytes = ::write(c.getWriteFd(), 
			c.m_request_data.m_body.c_str() + c.m_cgi_write_offset,
			len);

	if (nbytes == -1) {
		throw HTTPError("Cgi::write: ", strerror(errno), 500);
	}
	if (static_cast<size_t>(nbytes) == len) {
		close(c.m_cgi_write_pipe[OUT]);
		c.m_cgi_write_pipe[OUT] = -1;
	}
	else {
		c.m_cgi_write_offset += nbytes;
	}
}

void	Cgi::setParentIo(t_client &c) {
	if (c.m_cgi_post) {
		if (pipe(c.m_cgi_write_pipe) == -1) {
			throw HTTPError("Cgi::run: pipe(cgi_write_pipe)", strerror(errno), 500);
		}
		if (c.m_cgi_write_pipe[OUT] > c.m_range_fd)
			c.m_range_fd = c.m_cgi_write_pipe[OUT];
	}
	if (pipe(c.m_cgi_read_pipe) == -1) {
		throw HTTPError("Cgi::run: pipe(cgi_read_pipe)", strerror(errno), 500);
	}
	if (fcntl(c.m_cgi_read_pipe[IN], F_SETFL, O_NONBLOCK) == -1)
		std::cout << "Cgi::init : fcntl(m_cgi_read_pipe[IN]): " << strerror(errno)<<std::endl;
	if (fcntl(c.m_cgi_read_pipe[OUT], F_SETFL, O_NONBLOCK) == -1)
		std::cout << "Cgi::init : fcntl(m_cgi_read_pipe[OUT]): " << strerror(errno)<<std::endl;
	if (c.m_cgi_read_pipe[IN] > c.m_range_fd)
		c.m_range_fd = c.m_cgi_read_pipe[IN];
}

void	Cgi::setChildIo(t_client &c) {
	if (dup2(c.m_cgi_read_pipe[OUT], STDOUT_FILENO) == -1) {
		throw HTTPError("Cgi::exec: dup2(cgi_read_pipe[OUT], STDOUT_FILENO)", strerror(errno), 500);
	}
	if (close(c.m_cgi_read_pipe[IN]) == -1
		|| close(c.m_cgi_read_pipe[OUT]) == -1)
		throw HTTPError("Cgi::setChildIo: close(cgi_read_pipe[IN-OUT])", strerror(errno), 500);
	if (c.m_cgi_post) {
		if (dup2(c.m_cgi_write_pipe[IN], STDIN_FILENO) == -1) {
			throw HTTPError("Cgi::exec: dup2(cgi_write_pipe[IN], STDIN_FILENO)", strerror(errno), 500);
		}
	if (close(c.m_cgi_write_pipe[IN]) == -1
		|| close(c.m_cgi_write_pipe[OUT]) == -1)
		throw HTTPError("Cgi::setChildIo: close(cgi_write_pipe[IN-OUT])", strerror(errno), 500);
	}
}


void	Cgi::exec(t_client &c) {
	std::cout<<"exec"<<std::endl;
	if ((c.m_cgi_pid = fork()) == -1) {
		this->clear();
		throw HTTPError("Cgi::exec: fork", strerror(errno), 500);
	}
	if (!c.m_cgi_pid) {
		this->setChildIo(c);
		if (execve(this->m_argv[0], this->m_argv, this->m_env_array) == -1) {
			this->clear();
			throw HTTPError("Cgi::exec: execve", strerror(errno), 500);
		}
	}
	if (close(c.m_cgi_read_pipe[OUT]) == -1)
		throw HTTPError("Cgi::exec: close(m_cgi_read_pipe[OUT])", strerror(errno), 500);
	c.m_cgi_read_pipe[OUT] = -1;
}

void	Cgi::run(t_client &c) {
	std::cout<<"run cgi"<<std::endl;
	if (!c.m_cgi_running) {
		c.m_cgi_running = true;
		c.m_cgi_post = c.m_request_data.m_method == POST;
		this->init();
		this->fillEnv(c.m_request_data);
		this->convertEnv(c);
		this->setParentIo(c);
		this->exec(c);
	}
}

void	Cgi::fillEnv(t_request_data &request) {
	char	buf[PATH_MAX];
	(void)buf;
	this->m_env_map["AUTH_TYPE"]=std::string(request.m_headers[AUTHORIZATION]);
	this->m_env_map["CONTENT_LENGTH"]=ft::intToString(request.m_body.size());
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
	this->m_env_map["SERVER_NAME"] =SERVER_VERSION;
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

void	Cgi::generateResponse(t_client &c) {
	if (!c.m_response_data.m_cgi_metadata_parsed) {
		//tranfer headers from output buf to response struct
		//check for valid header
		//transfer headers from response struct to response_str
		size_t	metadata_index = ft::fullMetaData(c.m_cgi_out_buf);
		if (metadata_index == std::string::npos)
			return ;
		c.m_response_str.append(c.m_cgi_out_buf, 0, metadata_index + CRLF_LEN);
		c.m_response_str.append(CRLF);
		c.m_response_data.m_cgi_metadata_parsed = true;
		c.m_cgi_out_buf.erase(0, metadata_index + CRLF_LEN);
	}
	if (c.m_response_data.m_cgi_metadata_sent) {
		if (c.m_cgi_out_buf.size() == 0)
			c.m_cgi_end_chunk = 1;
		c.m_response_str.append(ft::hexString(c.m_cgi_out_buf.size()) + CRLF + c.m_cgi_out_buf + CRLF);
		c.m_cgi_out_buf.clear();
	}
}

void	RequestHandler::handleCgiMetadata(t_request &request, std::string &file) {
	request.m_cgi = true;
	if (request.m_real_path.size() - file.size() == 0) {
		request.m_file = file;
		return ;
	}
	size_t query_string_index = request.m_file.find('?', 0);
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
	if (!(request.m_method == GET || request.m_method == HEAD || request.m_method == POST))
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


void	Cgi::setCgiFd(fd_set *read_set, fd_set *write_set, t_client &c) {
	FD_ZERO(read_set);
	FD_ZERO(write_set);
	if (!c.m_cgi_end_chunk)
		FD_SET(c.m_cgi_read_pipe[IN], read_set);
	if (c.m_cgi_post && c.m_cgi_write_pipe[OUT] != -1)
		FD_SET(c.m_cgi_write_pipe[OUT], write_set);
}

int RequestHandler::handleCgi(t_client &c) {
	pid_t	wpid = 0;
	int		wstatus = 0;
	fd_set	read_set, write_set;
	int	&read_fd = c.getReadFd();
	int	&write_fd = c.getWriteFd();

	try {
		//wait
		if (c.m_cgi_running) {
			wpid = waitpid(c.m_cgi_pid, &wstatus, WNOHANG);
			if (wpid == -1)
				throw HTTPError("RequestHandler::handleCgi : wait", strerror(errno), 500);
			if (wpid) // has exited, maybe also check for WIFEXITED(), WSTATUS() here
				c.m_cgi_running = false;
		}
		//select
		this->m_cgi.setCgiFd(&read_set, &write_set, c);
		if (select(c.m_range_fd + 1, &read_set, &write_set, NULL, 0) == -1)
			throw HTTPError("RequestHandler::handleCgi: select", strerror(errno), 500);
		//write
		if (c.m_cgi_post && FD_ISSET(write_fd, &write_set)) {
			this->m_cgi.write(c);
		}
		//read
		if (!FD_ISSET(read_fd, &read_set))
			return CONTINUE;
		this->m_cgi.read(c);
		//generate response
		if (c.m_cgi_end_chunk) //just read last chunk
			this->m_cgi.stop(c);
		this->m_cgi.generateResponse(c);
		if (!c.m_response_data.m_cgi_metadata_parsed)
			return CONTINUE; //hasn't received fullmetadata
	}
	catch (HTTPError &e) {
		std::cerr << e.what() << std::endl;
		m_client->m_request_data.m_status_code = e.HTTPStatusCode();
	}
	return SUCCESS;
}
