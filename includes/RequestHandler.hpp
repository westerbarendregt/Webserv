#ifndef REQUEST_HANDLER
# define REQUEST_HANDLER

# include <map>
# include <string>
# include "Client.hpp"

class RequestHandler
{
	public:
		typedef	Client		t_client;
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

		std::string Content_Type();

		std::string statusLine();
		std::string responseBody();
		std::string responseHeaders();

		std::string	generateErrorPage(int error);

		std::map<int, std::string>	m_status_codes;
		void	initStatusCodes();

		std::map<std::string, std::string>	m_mime_types;
		void	initMimeTypes();

		std::vector<std::string>	m_response_headers;
};

#endif

