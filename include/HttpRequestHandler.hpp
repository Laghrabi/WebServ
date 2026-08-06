#ifndef HTTPREQUESTHANDLER_CPP
#define HTTPREQUESTHANDLER_CPP


#include "webserver.hpp"
#include "HttpRequest.hpp"
#include "HttpResponse.hpp"


class HttpRequestHandler
{
    private:
        HttpRequest& request;
        HttpResponse& response;
        
        HttpRequestHandler(const HttpRequestHandler& other):
        request(other.request), response(other.response)
        {
            (void) other;
        }
        HttpRequestHandler& operator=(const HttpRequestHandler& other) {
            (void) other;
            return *this;
        }
    public:
        std::string checkConnection();
        HttpRequestHandler(HttpRequest& req, HttpResponse& res) : request(req), response(res) {}
        ~HttpRequestHandler() {};
        void makeError(HttpStatus code);
        void handleRequest();
        void handleGet();
        void handlePost();
        void handleDelete();
        void serveFile();
        void generateAutoIndex();
        void makeRedirect();
        std::string generateAutoIndexHtml(const std::string& directoryPath);
        std::string generateErrorPage(HttpStatus code);

<<<<<<< HEAD
        void standardHeader(std::vector<char> buffer , std::string connection);
=======
        void standardHeader(std::vector<char>& buffer , std::string connection);
>>>>>>> 93faf3f4fa76347df37a672b8e064c00d48a0aab
};
    
#endif


