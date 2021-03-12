#ifndef REQUEST_HANDLER
# define REQUEST_HANDLER

# include <map>
# include <string>
# include "Client.hpp"
# include "VirtualServer.hpp"
# include "Cgi.hpp"
# include "Conf.hpp"
# include <sys/stat.h>
# include <limits.h>

class Client;

//do we need a class for Request Handler and a class Cgi, or simply a cgi section in the requesthandler class
class RequestHandler
{
	public:
		typedef	Client			t_client;
		typedef	Request			t_request;
		typedef	VirtualServer	t_v_server;
		typedef	s_v_server_conf::t_directives t_directives;
		typedef	s_v_server_conf::t_routes	t_routes;
		RequestHandler();
		~RequestHandler();
		void	handleMetadata(t_client &c);
		void	handleCgiMetadata(t_request &request);
		void	handleRequest(t_client &c);
		bool	validCgi(t_request &request, size_t extension_index);

		void printStatusCodes();
		void printMimeTypes();

	private:
			Cgi				m_cgi;
			struct	stat	m_statbuf;
		std::string handleGET();
		std::string handleHEAD();
		std::string handlePOST();
		std::string handlePUT();
		std::string handleDELETE();

		std::string Content_Length(std::string const & body);
		std::string Content_Type();
		std::string Server();

		std::string statusLine();
		std::string responseBody();
		std::string responseHeaders(std::string const & body);

		std::string	generateErrorPage(int error);

		std::map<int, std::string>	m_status_codes;
		void	initStatusCodes();

		std::map<std::string, std::string>	m_mime_types;
		void	initMimeTypes();

		std::vector<std::string>	m_response_headers;

		t_client	*m_client;
};

#endif

