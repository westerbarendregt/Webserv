#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "Conf.hpp"
#include "Error.hpp"
#include "RequestHandler.hpp"
#include "Server.hpp"
#include "utils.hpp"
#include "WebServer.hpp"
#include "Logger.hpp"

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
	return "Content-Language: " + this->m_response_data->m_content_language + CRLF;
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
	return "Content-Location: " + this->m_response_data->m_content_location + CRLF;
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
	return "Retry-After: 3600" CRLF;
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
	int fd = open(this->m_client->m_request_data.m_stat_file.c_str(), O_RDONLY);
	if (fd == -1) {
		throw HTTPError("RequestHandler::responseBody", "error opening file", 500);
	}
	char 	buf[1025];
	ssize_t ret;
	do {
		ret = read(fd, buf, 1024);
		if (ret == -1) {
			throw HTTPError("RequestHandler::responseBody", "error reading file", 500);
		}
		buf[ret] = '\0';
		this->m_response_data->m_body.append(buf, ret);
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

	 SetServer();
	 SetDate();
	 SetContentType();
	 SetContentLength();

	 std::string	response_headers = responseHeaders();

	 return status_line + response_headers + CRLF;
 }

 std::string RequestHandler::handlePOST() {
	std::string status_line = statusLine(200);
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
	std::string ret, error_path;
	std::vector<std::string>	v;

	if (this->m_client->m_v_server) {
		t_error_pages const & error_pages = this->m_client->m_v_server->m_configs.m_error_pages;
		for (t_error_pages::const_iterator it = error_pages.begin(); it != error_pages.end(); ++it) {
			v = ft::split(*it);
			if (!v.empty()) {
				error_path = v.back();
				v.pop_back();
			}
			for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {
				if (*it == ft::intToString(error)) {
					this->m_request_data->m_stat_file = error_path;
					try {
						if (stat(error_path.c_str(), &this->m_statbuf))
							throw HTTPError("RequestHandler::generateErrorPage ", "invalid default error page", 404);
						responseBody();
					} catch (HTTPError & e) {
						std::cerr << e.what() << std::endl;
						this->m_request_data->m_status_code = e.HTTPStatusCode();
						error = e.HTTPStatusCode();
						this->m_response_data->m_body.clear();
					}
					break;
				}
			}
		}
	}
	std::string status_line = statusLine(error);

	if (this->m_client->m_response_data.m_body.empty()) {
		this->m_response_data->m_body =
				"<html>" CRLF
				"<head><title>" + ft::intToString(error) + ' ' + m_status_codes[error] + "</title></head>" CRLF
				"<body>" CRLF
				"<center><h1>" + ft::intToString(error) + ' ' + m_status_codes[error] + "</h1></center>" CRLF
				"<hr><center>" + SERVER_VERSION + "</center>" CRLF
				"</body>" CRLF
				"</html>" CRLF
				;
	}

	this->m_client->m_response_data.m_content_type = "text/html";
	SetServer();
	SetDate();
	SetContentType();
	SetContentLength();
	if (error == 503) SetRetryAfter();
	std::string	response_headers = responseHeaders();
	ret = status_line + response_headers + CRLF;
	if (m_request_data->m_method != HEAD)
		ret.append(this->m_response_data->m_body);
	return ret;
}

//appends index and updates m_real_path in member request
void	RequestHandler::formatIndex(std::string &stat_file) {
	if (!this->m_request_data)
		return ;

	struct	stat statbuf;
	t_request	& request = *this->m_request_data;
	std::string & real_path = request.m_real_path;

	if (request.m_location->second.count("index") == 1) {
		if ((request.m_file_type & S_IFMT) == S_IFDIR) {
			if (stat_file[stat_file.size() - 1] != '/')
				stat_file.append("/");
			std::string const &index = request.m_location->second["index"];
			std::vector <std::string> v = ft::split(index);
			bool index_exist = false;
			for (std::vector<std::string>::iterator it = v.begin(); it != v.end(); ++it) {

				std::string path_index = stat_file + *it;
				Logger::Log() << "path_index: " << path_index << std::endl;
				if (stat(path_index.c_str(), &statbuf) == 0 &&
					(statbuf.st_mode & S_IFMT) == S_IFREG) {
					request.m_file_type = statbuf.st_mode;
					index_exist = true;
					if (real_path.size() < path_index.size())
						real_path = path_index;
					stat_file = path_index;
					size_t	query_string =  request.m_path.find('?');
					if (query_string != std::string::npos)
						request.m_path.insert(query_string, *it);
					else
						request.m_path.append(*it);
					Logger::Log() << "real_path: " << real_path << std::endl;
					break;
				}
			}
			if (!index_exist) {
				throw HTTPError("RequestHandler::handleMetadata", "index not found", 404);
			}
		}
	}

}

//if file, gets extension, then detects cgi or sets mime types
//if directory, 
void	RequestHandler::interpretUri(std::string &stat_file) {
	if (!this->m_request_data || !this->m_client)
		return ;

	t_request & request = *this->m_request_data;
	t_response & response = this->m_client->m_response_data;
	std::string const & file = request.m_file;
	std::string const & real_path = request.m_real_path;
	int	method = request.m_method;


	if (method == POST || (request.m_file_type & S_IFMT) == S_IFREG) { //file
		// 	extract extension
		size_t	extension = file.find_last_of('.', file.size());
		Logger::Log()<<"extension index: "<<extension<<std::endl;
		request.m_path_info = request.m_path;
		request.m_stat_file = stat_file;
		size_t query_string_index = request.m_path.find('?', 0);
		if (query_string_index != std::string::npos && query_string_index != request.m_path.size() - 1) { 
			request.m_query_string = request.m_path.substr(query_string_index + 1, std::string::npos);
			request.m_path_info.resize(query_string_index);
		}
		if (extension == std::string::npos) {
			response.m_content_type = "text/plain";
		}
		else if (this->validCgi(request, extension)) {
			Logger::Log()<<"cgi detected"<<std::endl;
			request.m_cgi = true;
			return ;
		}
		else {
			response.m_content_type = this->m_mime_types[stat_file.substr(stat_file.rfind('.') + 1)];
			Logger::Log() << "content-type: "<<this->m_client->m_response_data.m_content_type<<std::endl;
		}

		Logger::Log()<<"m_stat_file: "<<this->m_request_data->m_stat_file<<std::endl;
		Logger::Log()<<"m_path_info: "<<this->m_request_data->m_path_info<<std::endl;
		Logger::Log()<<"m_query_string : "<<this->m_request_data->m_query_string<<std::endl;
		request.m_path_info.resize(request.m_path_info.size() - request.m_path_info.find(request.m_file) - request.m_file.size());
		if (!request.m_path_info.empty()) {
			throw HTTPError("RequestHandler::interpretUri", "path_info for non-cgi request", 404);
		}
		if (!request.m_query_string.empty() && request.m_method == POST) {
			throw HTTPError("RequestHandler::handleMetadata", "query string with non-cgi POST", 405);
		}
	}
	else if (method == PUT) {
		if ((request.m_file_type & S_IFMT) == S_IFDIR || real_path[real_path.size() - 1] == '/') {
			throw HTTPError("RequestHandler::handleMetadata::PUT", "file is a directory", 409);
		}
	}
	else if ((request.m_file_type & S_IFMT) == S_IFDIR) {
		if (request.m_location->second["autoindex"] == "on") {
			Logger::Log()<<"dir listing"<<std::endl;
			request.m_autoindex= true;
		}
		else
			throw HTTPError("RequestHandler::handleMetadata", "directory listing not enabled", 403);
	}
}

//returns path to identified resource and update file_type in member request
std::string	RequestHandler::statFile() {
	if (!this->m_request_data)
		return "";

	t_request & request = *this->m_request_data;
	size_t	prefix = 0, next_prefix = 0;
	std::string	stat_file;
	std::string const & real_path = request.m_real_path;
	int	method = request.m_method;
	struct	stat	statbuf;

	next_prefix = real_path.find("/?", 0);
	if (next_prefix != std::string::npos) {
		stat_file = real_path.substr(0, next_prefix);
		if (stat(stat_file.c_str(), &statbuf)) {
			if (method == POST || method == PUT) {
				request.m_file_type = TYPE_UNDEFINED;
			}
			request.m_file_type = TYPE_UNDEFINED;
			throw HTTPError("RequestHandler::handleMetadata: stat", "invalid full path", 404);
		}
		request.m_file_type = statbuf.st_mode;
		return stat_file;
	}
	while (prefix < real_path.size()) {
		prefix = real_path.find_first_of("/?", prefix);
		next_prefix = prefix == std::string::npos ? std::string::npos : real_path.find_first_of("?/", prefix + 1);
		stat_file = real_path.substr(0, next_prefix);
		Logger::Log()<<"\tstat "<<stat_file<<std::endl;
		if (stat(stat_file.c_str(), &statbuf)) {
			if (method == POST || method == PUT) {
				request.m_file_type = TYPE_UNDEFINED;
				break ;
			}
			request.m_file_type = TYPE_UNDEFINED;
			throw HTTPError("RequestHandler::handleMetadata: stat", "invalid full path", 404);
		}
		request.m_file_type = statbuf.st_mode;
		//also check permission
		if ((request.m_file_type & S_IFMT) == S_IFREG)
			break ;
		if ((request.m_file_type & S_IFMT) != S_IFDIR 
				&& ( request.m_file_type & S_IFMT) != S_IFLNK) // if something else than a directory/smlink
			throw HTTPError("RequestHandler::handleMetadata: stat", "invalid full path, not a file/directory/symlink", 404);
		if (next_prefix == std::string::npos)
			break ;
		prefix = next_prefix;
	}
	return stat_file;
}


void	RequestHandler::handleMetadata(t_client &c) {
	if (c.m_request_data.m_status_code)
		return ; //don't need to do anything if an error has been detected in RequestParser
	Logger::Log()<<"handling metadata.."<<std::endl;

	this->m_client = &c;
	this->m_request_data = &c.m_request_data;
	this->m_response_data = &c.m_response_data;
	try {
		//get virtual server and location
		m_client->updateServerConf();
		c.m_request_data.m_location = c.m_v_server->getLocation(c.m_request_data);
		c.m_request_data.m_owner = &c;

		Logger::Log()<<"-------FETCHED BLOCK-------\n\tLISTEN "
			<<c.m_v_server->m_configs.m_directives["listen"]
			<<"\n\tSERVER_NAME "<< m_client->m_v_server->m_configs.m_directives["server_name"]
			<<"\n\tLOCATION/ROUTE "<< m_client->m_request_data.m_location->first<<"\n-----------"<<std::endl;

		std::string &real_path =  c.m_request_data.m_real_path;
		real_path = c.m_request_data.m_path;
		//real_path = c.m_request_data.m_path.substr(0, this->m_request_data->m_path.find('?'));
		std::string const & location = c.m_request_data.m_location->first;
		std::string alias, index, stat_file;

		t_directives const &	location_directives = this->m_request_data->m_location->second;
		t_directives::const_iterator	it = location_directives.find("alias");
		if (it != location_directives.end()) {
			alias = it->second;
			Logger::Log() << "alias: " << it->second << std::endl;
		}

		it = location_directives.find("index");
		if (it != location_directives.end()) {
			Logger::Log()  << "index: "  << it->second << std::endl;
		}

		/*replacing location path by alias path*/
		char buf[PATH_MAX];
		if (alias.empty())
			alias = getcwd(buf,  PATH_MAX);
		if (alias[alias.size() - 1] != '/')
			alias.append("/");
		size_t	const replace_len = location.size() > real_path.size() ? real_path.size() : location.size();
		real_path.replace(0, replace_len, alias);

		//parse URI
		stat_file = this->statFile();

		if (c.m_request_data.m_method != PUT)
			this->formatIndex(stat_file);
		this->m_request_data->m_stat_file = stat_file;
		c.m_request_data.m_file = this->m_request_data->m_stat_file.substr(this->m_request_data->m_stat_file.rfind('/') + 1, std::string::npos);

		//Log
		Logger::Log() << "location: " << location << std::endl;

		Logger::Log()<<"m_path: "<<this->m_client->m_request_data.m_path<<std::endl;
		Logger::Log()<<"m_real_path: "<<c.m_request_data.m_real_path<<std::endl;
		Logger::Log()<<"m_file: "<<c.m_request_data.m_file<<std::endl;
		Logger::Log()<<"stat file: "<<c.m_request_data.m_stat_file<<std::endl;

		//
		this->interpretUri(this->m_request_data->m_stat_file);
		//handle headers
		this->AllowedMethods();
		this->Authenticated();
		this->GetCharset();
		this->GetLanguage();
		this->UserAgent();
		//could be member of RequestHandler and called like this->AllowedMethods()
	} 
	catch (HTTPError & e)
	{
		std::cerr << e.what() << std::endl;
		this->m_request_data->m_status_code = e.HTTPStatusCode();
		this->m_request_data->m_done = true;
	}
}

std::string		RequestHandler::handlePUT()
{
	//I think here you can use this->m_request_data->m_file
	//std::string m_file = this->m_request_data->m_real_path.substr(this->m_request_data->m_real_path.find_last_of('/') + 1);
	std::string const & m_file = this->m_request_data->m_file;
	std::string upload_store = this->m_request_data->m_location->second["upload_store"];

	if (upload_store.find(" "))
		upload_store.erase(upload_store.find_last_of(' '));
	char* current_dir = getcwd(NULL, 0);
	if (chdir(upload_store.c_str()))
		throw HTTPError("RequestHandler::PUT", "Upload store directory doesn't exist", 500);

	if (stat(m_file.c_str(), &this->m_statbuf) == 0){
		this->m_request_data->m_status_code = 204;
		if ((this->m_statbuf.st_uid != getuid())) // checking if owner id of file is the same as the webserver id.
			throw HTTPError("RequestHandler::PUT", "Webserv is not the owner of this file", 403);
	}
	else 
		this->m_request_data->m_status_code = 201;

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
	std::string& max_body_server = m_client->m_v_server->m_configs.m_directives["client_max_body_size"];
	std::string& max_body_location = m_request_data->m_location->second["location_max_body_size"];
	size_t server_body_limit = ft::Atoi(max_body_server.c_str());
	size_t location_body_limit = ft::Atoi(max_body_location.c_str());
	// Logger::Log() << "server_limit: " << server_body_limit << std::endl;
	// Logger::Log() << "location_limit: " << location_body_limit << std::endl;

	if (server_body_limit && server_body_limit < m_request_data->m_content_length)
		throw HTTPError("RequestHandler::BodyLimit", "body size exceeded limit of server", 413);
	else if (location_body_limit && location_body_limit < m_request_data->m_content_length)
		throw HTTPError("RequestHandler::LocationLimit", "body size exceeded limit of location", 413);
	Logger::Log() << "[BODY SIZE = ALLOWED]"<< std::endl;
}

void	RequestHandler::handleRequest(t_client &c) {
	this->m_client = &c;
	this->m_request_data = &c.m_request_data;
	this->m_response_data = &c.m_response_data;
	try {
		if (m_request_data->m_status_code >= 400) {
			this->m_client->m_response_str = generateErrorPage(m_request_data->m_status_code);
			return ;
		}//first generate errorPage before doing any other check 
		CheckBodyLimits();
		if (m_request_data->m_cgi) {
			this->m_cgi.run(c);
		} else {
			switch (m_request_data->m_method) {
				case GET:
					m_client->m_response_str = handleGET();
					break;
				 case HEAD:
				 	m_client->m_response_str = handleHEAD();
				 	break;
				case PUT:
					m_client->m_response_str = handlePUT();
					break;
				case POST:
					m_client->m_response_str = handlePOST();
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
