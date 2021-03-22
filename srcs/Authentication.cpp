#include "Authentication.hpp"

void                AllowedMethods(Client& c)
{
    std::string allowed = c.getRequest().m_location->second["allow_method"]; // get this resource from allowed methods from the location
    if (allowed[0] == 0){
        std::cout << "[METHOD = ALLOWED]" << std::endl;
        return ;
    } // checking if location has an limited methods that are allowed
    std::string method = RequestParser::GetMethodString(c);
    if (allowed.find(method) == std::string::npos) // checking if used method is in allowed methods of location
        throw HTTPError("Allowed Methods", "Method not allowed", 405);
    std::cout << "[METHOD = ALLOWED]" << std::endl;
}

void                CheckCorrectCredentials(std::string decoded, std::string path_ht)
{
    int fd = open(path_ht.c_str(), O_RDONLY);
    std::cout << "printing path: " << path_ht.c_str() << "--done" << std::endl;
    if (fd == -1)
        throw HTTPError("Authentication", "Incorrect path to .htpasswd or .htpasswd couldn't be opended", 403);
    char buf[1001];
    int ret = read(fd, buf, 1000);
    if (close(fd) == -1 || ret == -1)
        throw HTTPError("Authentication", "couldn't read from .htpasswd", 403);
    std::string line, credentials;
    credentials = buf;
    // std::cout << credentials << std::endl;
    size_t start = 0;
    while (ft_getline(credentials, line, 0, start)){
        if (line.compare(decoded) == 0) // found match with credentials
            return ;
    }
    // std::cout << credentials << std::endl;
    throw HTTPError("Authentication", "No credential match was found...", 403); // credentials were located but were incorrect
}

void                Authenticated(Client& c)
{
    std::string path_ht = c.getRequest().m_location->second["auth_basic_user_file"]; // replace this with path to .htpasswd found in location! 
    if (path_ht[0] == 0) // checks if location needs authentication. 
        return ;
    std::string auth = c.getRequest().m_headers[AUTHORIZATION];
    // std::cout << "AUTH: " << auth << std::endl;
    if (auth == "") // checking if there is authentication. 
        throw HTTPError("Authentication", "Asking for Credentials", 401); // if none, sending an 401 which means asking for authentication.
    if (auth.find("Basic") == std::string::npos)
        throw HTTPError("Authentication", "SERVER only supports Basic authentication", 403); // authentication is not basic which server doesn't support
    std::string encoded, decoded;
    encoded = auth.substr(auth.find(' ') + 1);
    decoded = base64_decode(encoded); // decode the incoming username and password
    // std::cout << "encoded: " << encoded << std::endl;
    // std::cout << "decoded: " << base64_decode(encoded) << std::endl;
    CheckCorrectCredentials(decoded, path_ht);
    std::cout << "[USER-AGENT = AUTHENTICATED]" << std::endl;
}
