#ifndef HTTPREQUESTHANDLER_CPP
#define HTTPREQUESTHANDLER_CPP


#include "webserver.hpp"

class HttpRequestHandler
{
    private:
        HttpRequest& request;
        HttpResponse& response;
    
    public:
        HttpRequestHandler(HttpRequest& req, HttpResponse& res) : request(req), response(res) {}
        HttpRequestHandler& operator=(const HttpRequestHandler& other) {
            if (this != &other) {
                request = other.request;
                response = other.response;
            }
            return *this;
        }
        ~HttpRequestHandler() {}
        void handleRequest() {};
        void handleGet() {};
        void handlePost() {};
        void handleDelete() {};
};


void HttpRequestHandler::handleRequest()
{
    if  (request.getMethod() == "GET")
    {
        handleGet();
    }
    else if (request.getMethod() == "POST")
    {
        handlePost();
    }
    else if (request.getMethod() == "DELETE")
    {
        handleDelete();
    }
}

void HttpRequestHandler::handleGet()
{
    const RouteResult& result = request._routeResult;

    switch (result.action)
    {
        // case ACTION_SERVE_FILE:
        //     serveFile();
        //     break;w
        // case ACTION_SERVE_INDEX:
        //     serveFile();
        //     break;
        // case ACTION_AUTOINDEX:
        //     generateAutoIndex();
        //     break;
        // case ACTION_REDIRECT:
        //     response.makeRedirect();
        //     break;
        // case ACTION_ERROR:
        //     response.makeError();
        //     break;
        // default:
        //     response.makeError();
        //     break;
    }
}

void HttpRequestHandler::handlePost()
{

}
#endif