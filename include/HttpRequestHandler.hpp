#ifndef HTTPREQUESTHANDLER_CPP
#define HTTPREQUESTHANDLER_CPP


#include "webserver.hpp"


#define server_name "1337-webserver"

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
        void makeError(int code);
        void handleRequest() {};
        void handleGet() {};
        void handlePost() {};
        void handleDelete() {};
        void serveFile();
        void generateAutoIndex();
        void makeRedirect();
        std::string HttpRequestHandler::generateAutoIndexHtml(const std::string& directoryPath);
};
    
#endif

// in the connection manager
// switch(response.getBodyType())
// {
    //     case BODY_MEMORY:
    
    //         sendMemory();
    
    //         break;
    
    //     case BODY_FILE:
    
    //         sendFile();

//         break;

//     default:

//         break;
// }

