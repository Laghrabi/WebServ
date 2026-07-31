#ifndef HTTPREQUESTHANDLER_CPP
#define HTTPREQUESTHANDLER_CPP


// #include "HttpRequest.hpp"
#include "webserver.hpp"

#define SERVER_NAME "1337-webserver"

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
};
    
#endif


