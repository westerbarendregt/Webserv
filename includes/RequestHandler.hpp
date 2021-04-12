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

std::string         EncodePassword(std::string& password);

//do we need a class for Request Handler and a class Cgi, or simply a cgi section in the requesthandler class
class RequestHandler
{
	public:
		typedef	Client			t_client;
		typedef	Request			t_request;
		typedef	Response		t_response;
		typedef	VirtualServer	t_v_server;
		typedef	s_v_server_conf::t_directives t_directives;
		typedef	s_v_server_conf::t_routes	t_routes;

		friend class Server;

		RequestHandler();
		~RequestHandler();
		void				handleMetadata(t_client &c);
		std::string			statFile();
		void				formatIndex(std::string &stat_file);
		void				interpretUri(std::string & stat_file);
		void				handleCgiMetadata(t_request &request, std::string &file);
		void				handleCgiResponse(t_client &c);
		int					handleCgi(t_client &c);
		void				handleRequest(t_client &c);
		bool				validCgi(t_request &request, size_t extension_index);
		void                Authenticated();
		void                AllowedMethods();
		void                GetLanguage();
		void				languageHeaders(std::string extension);
		void                GetCharset();
		void				charsetHeaders(std::string extension);


		void 				printStatusCodes();
		void 				printMimeTypes();

		void 				SetAllow();
		void 				SetContentLanguage();
		void 				SetContentLength();
		void				SetContentLocation();
		void				SetContentType();
		void				SetDate();
		void				SetLastModified();
		void				SetRetryAfter();
		void				SetServer();
		void				SetLocation();
		void				SetTransferEncoding();
		void				SetWWWAuthenticate();

		std::string			GetAllow();
		std::string 		GetContentLanguage();
		std::string 		GetContentLength();
		std::string 		GetContentLocation();
		std::string 		GetContentType();
		std::string 		GetDate();
		std::string 		GetLastModified();
		std::string 		GetLocation();
		std::string 		GetRetryAfter();
		std::string 		GetServer();
		std::string 		GetTransferEncoding();
		std::string 		GetWWWAuthenticate();

	private:
		Cgi					m_cgi;
		void				setCgiFd(fd_set *read_set, fd_set *write_set, t_client &c);
		std::string 		handleGET();
		std::string 		handleHEAD();
		std::string 		handlePOST();
		std::string 		handlePUT();
		struct stat			m_statbuf;
		// std::string handleDELETE();


		void	handleAutoIndex();


		std::string 		statusLine(int status_code = 0);
		void				responseBody();
		std::string 		responseHeaders();
		void				CheckBodyLimits();

		std::string			generateErrorPage(int error);

		std::map<int, std::string>	m_status_codes;
		void				initStatusCodes();

		std::map<std::string, std::string>	m_mime_types;
		void				initMimeTypes();

		t_client	*m_client;
		t_request	*m_request_data;
		Response	*m_response_data;
};

#endif

