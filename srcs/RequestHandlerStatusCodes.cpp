#include "RequestHandler.hpp"

void	RequestHandler::initStatusCodes() {

	m_status_codes[100] = "Continue";
	m_status_codes[101] = "Switching Protocols";

	m_status_codes[200] = "OK";
	m_status_codes[201] = "Created";
	m_status_codes[202] = "Accepted";
	m_status_codes[203] = "Non-Authorative Information";
	m_status_codes[204] = "No Content";
	m_status_codes[205] = "Reset Content";
	m_status_codes[206] = "Partial Content";

	m_status_codes[300] = "Multiple Choices";
	m_status_codes[301] = "Moved Permanently";
	m_status_codes[302] = "Found";
	m_status_codes[303] = "See Other";
	m_status_codes[304] = "Not Modified";
	m_status_codes[305] = "Use Proxy";
	m_status_codes[307] = "Temporary Redirect";

	m_status_codes[400] = "Bad Request";
	m_status_codes[401] = "Unauthorized";
	m_status_codes[402] = "Payment Required";
	m_status_codes[403] = "Forbidden";
	m_status_codes[404] = "Not Found";
	m_status_codes[405] = "Method Not Allowed";
	m_status_codes[406] = "Not Acceptable";
	m_status_codes[407] = "Proxy Authentication Required";
	m_status_codes[408] = "Request Timeout";
	m_status_codes[409] = "Conflict";
	m_status_codes[410] = "Gone";
	m_status_codes[411] = "Length Required";
	m_status_codes[412] = "Precondition Failed";
	m_status_codes[413] = "Payload Too Large";
	m_status_codes[414] = "URI Too Long";
	m_status_codes[415] = "Unsupported Media Type";
	m_status_codes[416] = "Range Not Satisfiable";
	m_status_codes[417] = "Expectation Failed";
	m_status_codes[426] = "Upgrade Required";

	m_status_codes[500] = "Internal Server Error";
	m_status_codes[501] = "Not Implemented";
	m_status_codes[502] = "Bad Gateway";
	m_status_codes[503] = "Service Unavailable";
	m_status_codes[504] = "Gateway Timeout";
	m_status_codes[505] = "HTTP Version Not Supported";
}

void	RequestHandler::printStatusCodes() {
	std::map<int, std::string>::iterator	it;

	for (it = m_status_codes.begin(); it != m_status_codes.end(); ++it) {
		std::cout << it->first << " => " << m_status_codes[it->first] << std::endl;
	}
}
