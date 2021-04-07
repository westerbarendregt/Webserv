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
    if (close(fd) == -1 || ret == -1)
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

void                RequestHandler::languageHeaders(std::string extension){
    std::string &new_path = this->m_response_data->m_content_location;
    new_path = m_request_data->m_path + "/" + m_request_data->m_file + "." + extension;

    this->m_response_data->m_content_language = extension;
    SetContentLanguage();
    SetContentLocation();
    return ;
}

void                 RequestHandler::GetLanguage()
{
	struct	stat	statbuf;
    const char *language_extensions[] = {"nl", "fr", "de", "jp", "ch", "en", "po", "es", 0};
    std::string content_language = m_request_data->m_headers[ACCEPTLANGUAGE];

    if (content_language.empty()){
		Logger::Log() << "[NO LANGUAGE SPECIFIED]" << std::endl; // checking if content-language header is sent with request
        return ;
    }
    std::string real_path = m_request_data->m_real_path;
    std::vector<std::string> languages = ft::split(content_language, ',');
    for (std::vector<std::string>::iterator it = languages.begin(); it != languages.end(); ++it){
        (*it).erase(std::remove((*it).begin(), (*it).end(), ' '), (*it).end()); // stripping spaces
        if ((*it)[2] == '-' || (*it)[2] == ';' || (*it).size() == 2){
            Logger::Log() << "extension:[" << *it << "]" << std::endl;
            (*it) = (*it).substr(0, 2);
            if (stat((real_path + '.' + *it).c_str(), &statbuf) == 0){
                m_request_data->m_real_path = real_path + '.' + *it;
		        Logger::Log() << "[FOUND LANGUAGE SPECIFIED]" << std::endl;
                return languageHeaders(*it);
            }
        }
        if ((*it) == "*"){
            for (int i = 0; language_extensions[i]; ++i){
                if (stat((real_path + '.' + language_extensions[i]).c_str(), &statbuf) == 0){
                m_request_data->m_real_path = real_path + '.' + language_extensions[i];
		        Logger::Log() << "[FOUND LANGUAGE SPECIFIED]" << std::endl;
                return languageHeaders(language_extensions[i]);
                }
            }
        }
    }
}
