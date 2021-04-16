#include "RequestHandler.hpp"
#include "Logger.hpp"
#include "RequestParser.hpp"
#include "Error.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void                RequestHandler::AllowedMethods()
{
    std::string allowed = m_request_data->m_location->second["allow_method"]; // get this resource from allowed methods from the location
    if (allowed.empty()){
        Logger::Log() << "[ALL METHODS ALLOWED]" << std::endl;
        return ;
    } // checking if location has an limited methods that are allowed
    std::string method = RequestParser::GetMethodString(*m_client);
    if (allowed.find(method) == std::string::npos) // checking if used method is in allowed methods of location
    {
        SetAllow();
        // if (c.getRequest().m_method == HEAD)
        //     throw HTTPError("Allowed Methods", "Method not allowed", 404);
        throw HTTPError("Allowed Methods", "Method not allowed", 405);
    }
    Logger::Log() << "[METHOD = ALLOWED]" << std::endl;
}

void                CheckCorrectCredentials(std::string decoded, std::string path_ht)
{
    int fd = open(path_ht.c_str(), O_RDONLY);
    // Logger::Log() << "printing path: " << path_ht.c_str() << std::endl;
    if (fd == -1)
        throw HTTPError("Authentication", "Incorrect path to .htpasswd or .htpasswd couldn't be opended", 403);
    char buf[1001];
    int ret = read(fd, buf, 1000);
    if (ret == -1 || close(fd) == -1)
        throw HTTPError("Authentication", "couldn't read from .htpasswd", 403);
    std::string line, credentials;
    credentials = buf;
    size_t start = 0;
    while (ft::getline(credentials, line, 0, start)){
        if (line.compare(decoded) == 0) // found match with credentials
            return ;
    }
    throw HTTPError("Authentication", "No credential match was found...", 403); // credentials were located but were incorrect
}

void                RequestHandler::Authenticated()
{
    std::string path_ht = m_request_data->m_location->second["auth_basic_user_file"]; // replace this with path to .htpasswd found in location! 
    if (path_ht[0] == 0){
		Logger::Log() << "[NO AUTHENTICATION NEEDED]" << std::endl;
        return ;
    } // checks if location needs authentication. 
    std::string auth = m_request_data->m_headers[AUTHORIZATION];
    if (auth == ""){ // checking if there is authentication. 
        SetWWWAuthenticate();
        throw HTTPError("Authentication", "Asking for Credentials", 401); // if none, sending an 401 which means asking for authentication.
    }
    if (auth.find("Basic") == std::string::npos)
        throw HTTPError("Authentication", "SERVER only supports Basic authentication", 403); // authentication is not basic which server doesn't support
    std::string encoded, decoded;
    encoded = auth.substr(auth.find(' ') + 1);
    decoded = base64::decode(encoded); // decode the incoming username and password
	this->m_request_data->m_remote_user = decoded.substr(0, decoded.find(':'));
    CheckCorrectCredentials(decoded, path_ht);
	Logger::Log() << "[USER-AGENT = AUTHENTICATED]" << std::endl;
}

void                RequestHandler::charsetHeaders(std::string extension){
    std::string &new_path = this->m_response_data->m_content_location;
    new_path = this->m_path + "/" + m_request_data->m_file + "." + extension;
    SetContentLocation();

    if (this->m_client->m_response_data.m_content_type.empty())
		this->m_client->m_response_data.m_content_type += "text/plain";
    this->m_client->m_response_data.m_content_type += "; charset=";
    if (extension == "utf")
        this->m_client->m_response_data.m_content_type += "utf-8";
    else 
        this->m_client->m_response_data.m_content_type += "iso-8859-1";
}

void                 RequestHandler::GetCharset()
{
	struct	stat	statbuf;
    const char *charset_extensions[] = {"utf", "iso", 0};
    std::string charset_header = m_request_data->m_headers[ACCEPTCHARSET];

    if (charset_header.empty()){
		Logger::Log() << "[NO CHARSET SEPCIFIED]" << std::endl; // checking if charset header is sent with request
        return ;
    }
    const std::string& stat_file = m_request_data->m_stat_file;
    std::vector<std::string> charsets = ft::split(charset_header, ',');
    for (std::vector<std::string>::iterator it = charsets.begin(); it != charsets.end(); ++it){
        (*it).erase(std::remove((*it).begin(), (*it).end(), ' '), (*it).end()); // stripping spaces
        if ((*it).size() > 3 && (*it)[3] == '-'){
            (*it) = (*it).substr(0, 3);
            Logger::Log() << "extension:[" << *it << "]" << std::endl;
            if (stat((stat_file + '.' + *it).c_str(), &statbuf) == 0){
                m_request_data->m_stat_file = stat_file + '.' + *it;
		        Logger::Log() << "[FOUND CHARSET SPECIFIED]" << std::endl;
                return charsetHeaders(*it);
            }
        }
        if ((*it) == "*"){
            for (int i = 0; charset_extensions[i]; ++i){
                if (stat((stat_file + '.' + charset_extensions[i]).c_str(), &statbuf) == 0){
                m_request_data->m_stat_file = stat_file + '.' + charset_extensions[i];
		        Logger::Log() << "[FOUND CHARSET SPECIFIED]" << std::endl;
                return charsetHeaders(charset_extensions[i]);
                }
            }
        }
    }
}

void                RequestHandler::languageHeaders(std::string extension){
    std::string &new_path = this->m_response_data->m_content_location;
    new_path = this->m_path + "/" + m_request_data->m_file + "." + extension;

    this->m_response_data->m_content_language = extension;
    SetContentLanguage();
    SetContentLocation();
    return ;
}

void                 RequestHandler::GetLanguage()
{
	struct	stat	statbuf;
    const char *language_extensions[] = {"nl", "fr", "de", "jp", "ch", "en", "po", "es", 0};
    const std::string& content_language = m_request_data->m_headers[ACCEPTLANGUAGE];

    if (content_language.empty()){
		Logger::Log() << "[NO LANGUAGE SPECIFIED]" << std::endl; // checking if content-language header is sent with request
        return ;
    }
    const std::string& stat_file = m_request_data->m_stat_file;
    std::vector<std::string> languages = ft::split(content_language, ',');
    for (std::vector<std::string>::iterator it = languages.begin(); it != languages.end(); ++it){
        (*it).erase(std::remove((*it).begin(), (*it).end(), ' '), (*it).end()); // stripping spaces
        if ((*it).size() > 2 && ((*it)[2] == '-' || (*it)[2] == ';' || (*it).size() == 2)){
            Logger::Log() << "extension:[" << *it << "]" << std::endl;
            (*it) = (*it).substr(0, 2);
            if (stat((stat_file + '.' + *it).c_str(), &statbuf) == 0){
                m_request_data->m_stat_file = stat_file + '.' + *it;
		        Logger::Log() << "[FOUND LANGUAGE SPECIFIED]" << std::endl;
                return languageHeaders(*it);
            }
        }
        if ((*it) == "*"){
            for (int i = 0; language_extensions[i]; ++i){
                if (stat((stat_file + '.' + language_extensions[i]).c_str(), &statbuf) == 0){
                m_request_data->m_stat_file = stat_file + '.' + language_extensions[i];
		        Logger::Log() << "[FOUND LANGUAGE SPECIFIED]" << std::endl;
                return languageHeaders(language_extensions[i]);
                }
            }
        }
    }
}

void                 RequestHandler::UserAgent()
{
	struct	stat	statbuf;
    const std::string & user_agent = m_request_data->m_headers[USERAGENT];
    
    if (user_agent.empty()){
		Logger::Log() << "[NO USER AGENT]" << std::endl; // checking if content-language header is sent with request
        return ;
    }
    const std::string & stat_file = m_request_data->m_stat_file;
    size_t found1 = user_agent.find("curl"); //search for curl in user agent header and save index
    size_t found2 = user_agent.find("Safari"); // search for Safari in user agent header and save index
    size_t found3 = user_agent.find("Chrome"); // search for Safari in user agent header and save index
    if (found1 < found2 && found1 < found3)
        if (stat((stat_file + ".curl").c_str(), &statbuf) == 0)
            m_request_data->m_stat_file.append(".curl");
    if (found2 < found1 && found2 < found3)
        if (stat((stat_file + ".safari").c_str(), &statbuf) == 0)
            m_request_data->m_stat_file.append(".safari");
    if (found3 < found2 && found3 < found1)
        if (stat((stat_file + ".chrome").c_str(), &statbuf) == 0)
            m_request_data->m_stat_file.append(".chrome");
}
