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
        Session* session;
        
        HttpRequestHandler(const HttpRequestHandler& other);
        HttpRequestHandler& operator=(const HttpRequestHandler& other);
    public:
        std::string checkConnection();
        HttpRequestHandler(HttpRequest& req, HttpResponse& res, Session* ses);
        ~HttpRequestHandler();
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

        void standardHeader(std::vector<char>& buffer , std::string connection);
};
    
#endif