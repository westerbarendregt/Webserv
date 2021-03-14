#include "Authentication.hpp"

// std::string         EncodePassword(std::string& password)
// {
//     std::string encoded;
//     int index = 0;
//     for (int i = 0; password[i]; ++i){
//         if (password[i] > 100){
//             encoded[index] = password[i] - 10; 
//             index++;
//             encoded[index] = password[i] - 22;
//             index++;
//         }
//         else {
//             encoded[index] = password[i] + 10; 
//             index++;
//             encoded[index] = password[i] + 22;
//             index++;
//         }
//     }
//     return encoded;
// }
void                AllowedMethods(Client& c)
{
    std::string allowed = "GET, POST, HEAD"; // get this resource from allowed methods from the location
    if (allowed[0] == 0) // checking if location has an limited methods that are allowed
        return ;
    std::string method = RequestParser::GetMethodString(c);
    if (allowed.find(method) == std::string::npos) // checking if used method is in allowed methods of location
        throw HTTPError("Allowed Methods", "Method not allowed", 405);
}

void                CheckCorrectCredentials(std::string decoded, std::string path_ht)
{
    int fd = open(path_ht.c_str(), O_RDONLY);
    if (fd == -1)
        throw HTTPError("Authentication", "Incorrect path to .htpasswd or .htpasswd couldn't be opended", 403);
    //     std::cout << "Incorrect path to .htpasswd or .htpasswd couldn't be opended" << std::endl;
    //     return false;
    // }
    char buf[1001];
    int ret = read(fd, buf, 1000);
    if (ret == -1)
    // {
        // std::cout << "couldn't read from .htpasswd" << std::endl;
        throw HTTPError("Authentication", "couldn't read from .htpasswd", 403);
        // return false;
    // }
    std::string line, credentials;
    credentials = buf;
    size_t start = 0;
    while (ft_getline(credentials, line, 0, start)){
        // std::cout << "line:" << line << std::endl;
        if (line.compare(decoded) == 0) // found match with credentials
            return ;
    }
    // std::cout << "No credential match was found..." << std::endl;
    throw HTTPError("Authentication", "No credential match was found...", 403);
    // return false; // no match was found
}

void                Authenticated(Client& c)
{
    std::string path_ht = "test_conf/.htpasswd"; // replace this with path to .htpasswd found in location! 
    // bool is_auth = false;
    if (path_ht[0] == 0) // checks if location needs authentication. 
        return ;
    // std::cout << "----------------------SIZE:"<< c.getRequest().m_location->size() << std::endl;
    std::string auth = c.getRequest().m_headers[AUTHORIZATION];
    std::cout << "AUTH: " << auth << std::endl;
    if (auth == "") // checking if there is authentication. 
        throw HTTPError("Authentication", "Asking for Credentials", 401);

        // c.getRequest().m_error = 401; // if none, sending an 401 which means asking for authentication.
        // std::cout << "--------401---------" << std::endl;
        // return false; // throw
    
    if (auth.find("Basic") == std::string::npos)
        throw HTTPError("Authentication", "SERVER only supports Basic authentication", 401);

        // c.getRequest().m_error = 403; // authentication is not basic which server doesn't support
        // return false; // throw
    
    std::string name, password, encoded, decoded;
    encoded = auth.substr(auth.find(' ') + 1);
    decoded = base64_decode(encoded);
    std::cout << "encoded: " << encoded << std::endl;
    std::cout << "decoded: " << base64_decode(encoded) << std::endl;
    CheckCorrectCredentials(decoded, path_ht);
        // c.getRequest().m_error = 403; // credentials were located but were incorrect
        // return false; //throw
    // }
    // return true;
}

// void                Sethtpasswd
