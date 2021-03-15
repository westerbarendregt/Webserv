#ifndef REQUEST_HANDLER
# define REQUEST_HANDLER

# include <map>
# include <string>
# include "Client.hpp"
# include "VirtualServer.hpp"

class Client;

class RequestHandler
{
	public:
		typedef	Client			t_client;
		typedef VirtualServer	t_v_server;
		RequestHandler();
		~RequestHandler();
		void	handleMetadata(t_client &c);
		void	handleRequest(t_client &c);

		void printStatusCodes();
		void printMimeTypes();

	private:
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

