#ifndef HTTPRESPONSE_CPP
#define HTTPRESPONSE_CPP

#include "webserver.hpp"


// headers that i need to handle
// Content-Length
// Content-Type
// Date ----> Date: Tue, 25 Jul 2026 18:31:20 GMT
// Server
// Connection---->keep alive or close
// location in case of rediraction
//             HTTP/1.1 301 Moved Permanently
//             Location: /new-page
// Allow
//     405 Method Not Allowed
//     Allow: GET, POST


class HttpResponse
{
    private:
        std::string httpVersion;
        HttpStatus statusCode;
        std::string statusMessage;
        std::map<std::string, std::string> headers;
        std::vector<char> body;
        

    public:
        HttpResponse() : httpVersion("HTTP/1.1") {}
        HttpResponse(const HttpResponse& other): httpVersion(other.httpVersion), statusCode(other.statusCode),
            statusMessage(other.statusMessage), headers(other.headers), body(other.body) {}
        HttpResponse& operator=(const HttpResponse& other) {
            if (this != &other) {
                httpVersion = other.httpVersion;
                statusCode = other.statusCode;
                statusMessage = other.statusMessage;
                headers = other.headers;
                body = other.body;
            }
            return *this;
        }
        ~HttpResponse() {}
        std::vector<char> assembleResponse() const {
            std::vector<char> responseBuffer;
            std::string statusLine = httpVersion + " " + std::to_string(statusCode) + " " + statusMessage + "\r\n";
            responseBuffer.insert(responseBuffer.end(), statusLine.begin(), statusLine.end());
            for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
                std::string headerLine = it->first + ": " + it->second + "\r\n";
                responseBuffer.insert(responseBuffer.end(), headerLine.begin(), headerLine.end());
            }
            responseBuffer.insert(responseBuffer.end(), '\r');
            responseBuffer.insert(responseBuffer.end(), '\n');
            responseBuffer.insert(responseBuffer.end(), body.begin(), body.end());
            return responseBuffer;
        }
};




#endif