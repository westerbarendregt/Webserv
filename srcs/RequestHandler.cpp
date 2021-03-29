#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "Authentication.hpp"
#include "Conf.hpp"
#include "Error.hpp"
#include "RequestHandler.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "WebServer.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

RequestHandler::RequestHandler() {
	initStatusCodes();
	initMimeTypes();
}

RequestHandler::~RequestHandler() {}

void	RequestHandler::handleAutoIndex() {
	struct	stat	buf;
	struct	dirent*	d;
	struct	tm*	timeptr;
	char 	s[1025] = {};
	std::string&	body = this->m_client->m_response_data.m_body;
	std::string	link;

	DIR*	dir = opendir(this->m_client->m_request_data.m_real_path.c_str());
	if (dir == NULL) {
		throw HTTPError("opendir: ", "error opening directory", 500);
	}

	body += "<html>" CRLF;
	body += "<head><title>Index of /</title></head>" CRLF;
	body += "<body>" CRLF;
	body += "<h1>Index of ";
	body += this->m_client->m_request_data.m_path;
	body += "</h1><hr><pre>" CRLF;

	while ((d = readdir(dir)) != NULL) {
		std::string	name = d->d_name;
		std::string	local = this->m_client->m_request_data.m_real_path + d->d_name;

		if (name == ".") {
			continue;
		}
		if (stat(local.c_str(), &buf) == -1) {
			throw HTTPError("RequestHandler::handleAutoIndex ", strerror(errno), 500);
		}
		if (S_ISDIR(buf.st_mode) && name.length() < 50) {
			name.append(1, '/');
		}
		link = name;
		if (name.length() > 50) {
			name.resize(47);
			name.append("..>");
		}
		body += "<a href=\"";
		body += link;
		body += "\">" + name + "</a>";
		for (int i = name.length(); i < 51; ++i) {
			body.append(1, ' ');
		}
		timeptr = localtime(&buf.st_mtime);
		strftime(s, 1024, "%d-%b-%Y %R", timeptr);
		body.append(s);
		body.append("\t\t\t");
		if (S_ISDIR(buf.st_mode)) {
			body += '-';
		} else {
			body += ft::intToString(buf.st_size);
		}
		body += CRLF;
	}

	body += "</pre><hr></body>" CRLF;
	body += "</html>" CRLF;

	if (closedir(dir) == -1) {
		throw HTTPError("RequestHandler::handleAutoIndex ", strerror(errno), 500);
	}

	this->m_client->m_response_data.m_content_type = "text/html";
}

std::string RequestHandler::GetAllow() {
	std::string allowed = "Allow: "; // get this resource from allowed methods from the location
	allowed += this->m_client->m_request_data.m_location->second["allow_method"];
	return allowed + CRLF;
}

void		RequestHandler::SetAllow(){
	this->m_response_data->m_response_headers.push_back(GetAllow());
}

std::string RequestHandler::GetContentLanguage(){
	return "";
}

void		RequestHandler::SetContentLanguage(){
	this->m_response_data->m_response_headers.push_back(GetContentLanguage());
}

std::string RequestHandler::GetContentLength() {
	std::string& body = this->m_response_data->m_body;
	std::string content_length = "Content-Length: " + ft::intToString(body.length());
	return content_length + CRLF;
}

void		RequestHandler::SetContentLength(){
	this->m_response_data->m_response_headers.push_back(GetContentLength());
}

std::string RequestHandler::GetContentLocation(){
	return "";
}

void		RequestHandler::SetContentLocation(){
	this->m_response_data->m_response_headers.push_back(GetContentLocation());
}

std::string RequestHandler::GetContentType() {
	std::string	content_type = "Content-Type: ";

	content_type += this->m_client->m_response_data.m_content_type;

	if (this->m_client->m_response_data.m_content_type.empty()) {
		content_type += "text/plain";
	}
	return content_type + CRLF;
}

void		RequestHandler::SetContentType(){
	this->m_response_data->m_response_headers.push_back(GetContentType());
}

std::string RequestHandler::GetDate() {

	struct timeval	tp;
	std::string	date = "Date: ";

	if (gettimeofday(&tp, NULL) == -1) {
		throw HTTPError("RequestHandler::Date ", strerror(errno), 500);
	}
	date += ft::convertDate(&tp.tv_sec);
	return date + CRLF;
}

void		RequestHandler::SetDate(){
	this->m_response_data->m_response_headers.push_back(GetDate());
}

std::string RequestHandler::GetLastModified() {
	struct stat	buf;
	std::string	last_modified = "Last-Modified: ";

	if (stat(this->m_client->m_request_data.m_real_path.c_str(), &buf) == -1) {
		throw HTTPError("RequestHandler::Last_Modified ", strerror(errno), 500);
	}
	last_modified += ft::convertDate(&buf.st_mtime);
	return last_modified + CRLF;
}

void		RequestHandler::SetLastModified(){
	this->m_response_data->m_response_headers.push_back(GetLastModified());
}

std::string	RequestHandler::GetLocation(){
	std::string location = "Location: ";

	location += "http://";
	location += m_request_data->m_headers[HOST];
	location += m_request_data->m_path;
	// location += this->m_client->m_response_data.m_location;
	return location + CRLF;
}

void 		RequestHandler::SetLocation() {
	this->m_response_data->m_response_headers.push_back(GetLocation());
}

std::string RequestHandler::GetRetryAfter(){
	return "";
}

void		RequestHandler::SetRetryAfter(){
	this->m_response_data->m_response_headers.push_back(GetRetryAfter());
}

std::string RequestHandler::GetServer() {
	std::string	server = "Server: ";
	server += SERVER_VERSION;

	return server + CRLF;
}

void		RequestHandler::SetServer(){
	this->m_response_data->m_response_headers.push_back(GetServer());
}

std::string RequestHandler::GetTransferEncoding() {
	return "Transfer-Encoding: chunked" CRLF;
}

void		RequestHandler::SetTransferEncoding(){
	this->m_response_data->m_response_headers.push_back(GetTransferEncoding());
}

std::string RequestHandler::GetWWWAuthenticate() {
	std::string	www_authenticate = "WWW-Authenticate: Basic realm=";
	www_authenticate += this->m_client->m_request_data.m_location->second["auth_basic"]; // get from location
	www_authenticate += ", charset=\"UTF-8\"";

	return www_authenticate + CRLF;
}

void		RequestHandler::SetWWWAuthenticate(){
	this->m_response_data->m_response_headers.push_back(GetWWWAuthenticate());
}

std::string RequestHandler::statusLine(int status_code) {
	if (status_code == 0) {
		status_code = this->m_client->m_request_data.m_status_code;
	}

	std::string	status_line;

	status_line.append("HTTP/1.1 ");
	status_line.append(ft::intToString(status_code));
	status_line.append(1, ' ');
	status_line.append(m_status_codes[status_code]);

	return status_line + CRLF;
}

void 		RequestHandler::responseBody() {
	int fd = open(this->m_client->m_request_data.m_real_path.c_str(), O_RDONLY);
	if (fd == -1) {
		throw HTTPError("RequestHandler::responseBody", "error opening file", 500);
	}
	char 	buf[1025];
	ssize_t ret;
	do {
		std::fill_n(buf, 1025, 0);
		ret = read(fd, buf, 1024);
		if (ret == -1) {
			throw HTTPError("RequestHandler::responseBody", "error reading file", 500);
		}
		this->m_response_data->m_body.append(buf);
	} while (ret > 0);

	if (close(fd) == -1) {
		throw HTTPError("RequestHandler::responseBody", "error closing file", 500);
	}
}

std::string RequestHandler::responseHeaders()
{
	std::string	response_headers;
	std::vector<std::string>::iterator it = this->m_response_data->m_response_headers.begin();

	for (; it != this->m_response_data->m_response_headers.end(); ++it)
		response_headers.append(*it);

	return response_headers;
}

std::string RequestHandler::handleGET() {
	return handleHEAD() + this->m_response_data->m_body;
}

 std::string RequestHandler::handleHEAD() {

	 std::string status_line = statusLine(200);
	 if (this->m_client->m_request_data.m_autoindex) {
		 handleAutoIndex();
	 } else {
		 responseBody();
	 }

	 SetContentLength();
	 SetContentType();
	 SetDate();
	 SetServer();

	 std::string	response_headers = responseHeaders();

	 return status_line + response_headers + CRLF;
 }

 std::string RequestHandler::handlePOST() {
 	std::string status_line = statusLine(405);
	SetContentLength();
	SetContentType();
	SetDate();
	SetServer();
	std::string	response_headers = responseHeaders();
	return status_line + response_headers + CRLF;
 }

// std::string RequestHandler::handleDELETE() {
// 	std::string status_line = statusLine();
// 	std::string response_body = responseBody();
// 	std::string	response_headers = responseHeaders();
//
// 	return status_line + response_headers + CRLF + response_body;
// }

std::string	RequestHandler::generateErrorPage(int error) {
	std::string status_line = statusLine(error); 

	this->m_response_data->m_body = 
			"<html>" CRLF
			"<head><title>" + ft::intToString(error) + ' ' + m_status_codes[error] + "</title></head>" CRLF
			"<body>" CRLF
			"<center><h1>" + ft::intToString(error) + ' ' + m_status_codes[error] + "</h1></center>" CRLF
			"<hr><center>" + SERVER_VERSION + "</center>" CRLF
			"</body>" CRLF
			"</html>" CRLF
			;

	this->m_client->m_response_data.m_content_type = "text/html";
	SetServer();
	SetDate();
	SetContentType();
	SetContentLength();
	std::string	response_headers = responseHeaders();

	return status_line + response_headers + CRLF + this->m_response_data->m_body;
}

void	RequestHandler::handleMetadata(t_client &c) {
	if (c.m_request_data.m_status_code)
		return ; //don't need to do anything if an error has been detected in RequestParser
	std::cout<<"handling metadata.."<<std::endl;

	this->m_client = &c;
	this->m_request_data = &c.m_request_data;
	this->m_response_data = &c.m_response_data;
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
		std::string	&file = c.m_request_data.m_file;
		std::string	stat_file;
		real_path = c.m_request_data.m_path;
		std::string const & location = c.m_request_data.m_location->first;
		std::cout << "location: " << location << std::endl;
		std::string & alias = c.m_request_data.m_location->second["alias"];
		std::cout << "alias: " << alias << std::endl;
		std::cout << "index: " << this->m_client->m_request_data.m_location->second["index"] << std::endl;
		/*replacing location path by alias path (what if alias empty?)*/
		if (alias[alias.size() - 1] != '/')
			alias.append("/");
		real_path.replace(real_path.find(location), location.length(), alias);
		std::cout<<"real_path: "<<real_path<<std::endl;
		std::cout<<"path: "<<this->m_client->m_request_data.m_path<<std::endl;
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
			if (c.m_request_data.m_method != PUT)
				if (stat(stat_file.c_str(), &this->m_statbuf))
					throw HTTPError("RequestHandler::handleMetadata 1", "invalid full path", 404);
			//also check permission
			if ((this->m_statbuf.st_mode & S_IFMT) == S_IFREG) // if file
				break ;
			if (c.m_request_data.m_method != PUT)
				if ((this->m_statbuf.st_mode & S_IFMT) != S_IFDIR 
						&& (this->m_statbuf.st_mode & S_IFMT) != S_IFLNK) // if something else than a directory/smlink
					throw HTTPError("RequestHandler::handleMetadata 2", "invalid full path, not a file/directory/symlink", 404);
			if (next_prefix == std::string::npos)
				break ;
			prefix = next_prefix;
		}

		if ((this->m_statbuf.st_mode & S_IFMT) == S_IFDIR) {
			if (stat_file[stat_file.size() - 1] != '/')
				stat_file.append("/");
			std::string const &	index = this->m_client->m_request_data.m_location->second["index"];
			std::vector<std::string>	v = ft::split(index);
			for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {

				std::string	path_index = stat_file + *it;
				std::cout << "path_index: " << path_index << std::endl;
				if (stat(path_index.c_str(), &this->m_statbuf) == 0 && (this->m_statbuf.st_mode & S_IFMT) == S_IFREG) {
					real_path = path_index;
					std::cout << "real_path: " << real_path << std::endl;
					break;
				}
			}
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
				this->m_client->m_response_data.m_content_type = this->m_mime_types[file.substr(file.rfind('.') + 1)];
				std::cout << "content-type: "<<this->m_client->m_response_data.m_content_type<<std::endl;
				//	if there are additional entries after this file, we throw bad request
				if (c.m_request_data.m_method != PUT)
					if (next_prefix != std::string::npos)
						throw HTTPError("RequestHandler::handleMetadata", "invalid full path 3", 404);
			}
		} 
		//		see if autoindex on then flag it so handleRequest can call the appropriate method
		//		else return bad request?
		//		//we dont need to check if remaining entries, because loop on 141 doesn't stop at a directory unless it's the last prefix in uri
		else if (this->m_client->m_request_data.m_method != PUT)
		{
			if (c.m_request_data.m_location->second["autoindex"] == "on")
			{
				std::cout<<"dir listing"<<std::endl;
				c.m_request_data.m_autoindex= true;
			}
			else
				throw HTTPError("RequestHandler::handleMetadata", "directory listing not enabled", 403);
		}
		// std::cout<<"m_real_path: "<<c.m_request_data.m_real_path<<std::endl;
		// else if (c.m_request_data.m_method != PUT)
		// 	throw HTTPError("RequestHandler::handleMetadata", "directory listing not enabled", 403);

		// else 
		// 	throw HTTPError("RequestHandler::handleMetadata", "directory listing not enabled", 404);
		AllowedMethods(c, *this);
		Authenticated(c, *this);
		// std::cout<<"stat file: "<<stat_file<<std::endl;
		if (m_client->m_request_data.m_method == PUT)
			if (real_path[real_path.size() - 1] == '/' || (stat(real_path.c_str(), &this->m_statbuf) && S_ISDIR(this->m_statbuf.st_mode)))
				throw HTTPError("RequestHandler::handleMetadata::PUT", "file is a directory", 409);
	} catch (HTTPError & e) {
		std::cerr << e.what() << std::endl;
		m_client->m_request_data.m_status_code = e.HTTPStatusCode();
		m_client->m_request_data.m_done = true;
	}
}

std::string		RequestHandler::handlePUT()
{
	std::string m_file = this->m_request_data->m_real_path.substr(this->m_request_data->m_real_path.find_last_of('/') + 1);
	const char *upload_store = this->m_request_data->m_location->second["upload_store"].c_str();
	std::string path_to_file = std::string(upload_store) + m_file;

	if (stat(path_to_file.c_str(), &this->m_statbuf) == 0){
		this->m_request_data->m_status_code = 204;
		if ((this->m_statbuf.st_uid != getuid())) // checking if owner id of file is the same as the webserver id.
			throw HTTPError("RequestHandler::PUT", "Webserv is not the owner of this file", 403);
	}
	else 
		this->m_request_data->m_status_code = 201;
	char * current_dir = getcwd(NULL, 0);

	if (chdir(upload_store))
		throw HTTPError("RequestHandler::PUT", "Upload store directory doesn't exist", 500);
	int fd  = open(m_file.c_str(), O_TRUNC | O_CREAT | O_WRONLY,  0600); // S_IRWXU = owner having all persmissions 
	if (fd == -1)
		throw HTTPError("RequestHandler::PUT", "error creating file", 500);
	size_t written = write(fd, this->m_request_data->m_body.c_str(), this->m_request_data->m_content_length);
	if (close(fd) == -1)
		throw HTTPError("RequestHandler::PUT", "error closing file", 500);
	if (written != this->m_request_data->m_content_length)
		throw HTTPError("RequestHandler::PUT", "Didn't write the compleet file", 500);
	chdir(current_dir);

	SetServer();
	SetDate();
	if (this->m_request_data->m_status_code == 201){
		SetContentLength();
		SetLocation();
	}
	std::string status_line = statusLine();
	std::string	response_headers = responseHeaders();
	return status_line + response_headers + CRLF;
}

void	RequestHandler::CheckBodyLimits()
{
	std::cout << "1\n";
	std::string& max_body_server = m_client->m_v_server->m_configs.m_directives["client_max_body_size"];
	std::cout << "hello: "<< std::endl << max_body_server << std::endl;
	std::string& max_body_location = m_request_data->m_location->second["location_max_body_size"];
	
	std::cout << max_body_location << std::endl;
	size_t server_body_limit = ft::Atoi(max_body_server.c_str());
	std::cout << "1\n";
	size_t location_body_limit = ft::Atoi(max_body_location.c_str());
	std::cout << "1\n";
	std::cout << "server_limit: " << server_body_limit << std::endl;
	std::cout << "location_limit: " << location_body_limit << std::endl;

	if (server_body_limit && server_body_limit < m_request_data->m_content_length)
		throw HTTPError("RequestHandler::BodyLimit", "body size exceeded limit of server", 413);
	else if (location_body_limit && location_body_limit < m_request_data->m_content_length)
		throw HTTPError("RequestHandler::LocationLimit", "body size exceeded limit of location", 413);
	std::cout << "[BODY SIZE = ALLOWED]"<< std::endl;
}

void	RequestHandler::handleRequest(t_client &c) {
	try {
		// CheckBodyLimits();
		std::cout << "1\n";
		if (m_request_data->m_status_code >= 400) {
			this->m_client->m_response_str = generateErrorPage(m_request_data->m_status_code);
		std::cout << "2\n";
		} else if (m_request_data->m_cgi) {
			this->m_cgi.run(c);
		std::cout << "3\n";
		} else {
			switch (m_request_data->m_method) {
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
				// case DELETE:
				// 	m_client->m_response_str = handleDELETE();
				// 	break;
			}
		}
	} catch (HTTPError & e) {
		std::cerr << e.what() << std::endl;
		this->m_client->m_request_data.m_status_code = e.HTTPStatusCode();
		this->m_client->m_response_str = generateErrorPage(this->m_client->m_request_data.m_status_code);
	}
}
