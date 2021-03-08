#include "includes/RequestParser.hpp"
#include "includes/Client.hpp"

int main()
{
    Client c;

    c.testingRequest("POST /hello/ HTTP/1.1\ntransfer-encoding:   chunked  \n\n8\r\nhello he\r\n");

    std::cout << "parse return: " << RequestParser::Parse(c) << std::endl;
    c.testingRequest("0\r\n\r\n");
    std::cout << "parse return: " << RequestParser::GetBody(c) << std::endl;
    std::cout << "error code = " << c.testGetError() << std::endl;
    std::cout << c.getRequest().m_headers[TRANSFERENCODING]<< std::endl;
    std::cout << c.getRequest().m_body << std::endl;
    if (c.getRequest().m_done)
        std::cout << "done" << std::endl;
    return 0;
}