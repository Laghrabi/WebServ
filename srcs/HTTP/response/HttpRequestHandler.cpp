#include "webserver.hpp"

static std::string getCurrentDate()
{
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return std::string(buf);
}

void HttpRequestHandler::serveFile()
{
    const RouteResult& result = request._routeResult;
    const std::string& filePath = result.targetPath;
    struct stat st;
    stat(filePath.c_str(), &st);
    
    if (stat(filePath.c_str(), &st) == -1)
    {
        makeError(404);
        return;
    }
    if (!S_ISREG(st.st_mode))
    {
        makeError(403);
        return;
    }
    response.setBodySource(BODY_FILE);
    response.setFilePath(filePath);
    response.setStatusCode(result.statusCode);//im gonna wait for my partner to implement the getStatusMessage map
    response.setStatusMessage("OK");//also here
    response.setContentLength(st.st_size);
    // response.setHeader("Content-Type", getMimeType(filePath));//wtabnasba lhadi ana gha guessit kighatkon
    response.setHeader("Content-Length", std::to_string(st.st_size));
    response.setHeader("Connection", "keep-alive");
    response.setHeader("Date", getCurrentDate());
    response.setHeader("server", server_name);
}

std::string HttpRequestHandler::generateAutoIndexHtml(const std::string& directoryPath)
{
    //hamzaaa haaa l3ar lamasawblina chi html yr7am bok rah ma3reftch kindirlo
    //HELP ME PLEASE
    
}

void HttpRequestHandler::generateAutoIndex()
{
    const RouteResult& result = request._routeResult;
    const std::string& directoryPath = result.targetPath;
    
    std::string autoIndexHtml = generateAutoIndexHtml(directoryPath);
    response.setBodySource(BODY_BUFFER);
    response.setBufferBody(std::vector<char>(autoIndexHtml.begin(), autoIndexHtml.end()));
    response.setStatusCode(200);
    response.setStatusMessage("OK");
    response.setHeader("Content-Type", "text/html");
    response.setHeader("Content-Length", std::to_string(autoIndexHtml.size()));
    response.setHeader("Connection", "keep-alive");
    response.setHeader("Date", getCurrentDate());
    response.setHeader("server", server_name);
}

void HttpRequestHandler::makeRedirect()
{
    const RouteResult& result = request._routeResult;
    response.setStatusCode(result.statusCode);
    response.setStatusMessage("Found");
    response.setHeader("Location", result.targetPath);
    response.setBodySource(BODY_NONE);
    response.setHeader("Content-Length", "0");
    response.setHeader("Connection", "keep-alive");
    response.setHeader("Date", getCurrentDate());
    response.setHeader("server", server_name);
}

void HttpRequestHandler::makeError(int code)
{
    response.setStatusCode(code);
    // response.setStatusMessage(getStatusMessage(code));//i will wait for my partner to implement the getStatusMessage map
    //here i should check if there is a custom error page for this code and if yes i should set the filebody to that page
    //if no i will creat i simple html error page with the code and the message
    response.setBodySource(BODY_NONE);
    // if (code == 403) //NEED TO KNOW IS IT FORBIDDEN METHODS OR JUST FORBIDDEN ACCESS TO THE RESOURCE
        // response.setHeader("Allow", "GET, POST, DELETE");  how can i know the allowed methods for this route?
    response.setHeader("Content-Length", "0");
    response.setHeader("Connection", "keep-alive");
}

void HttpRequestHandler::handleGet()
{
    const RouteResult& result = request._routeResult;
    
    switch (result.action)
    {
        case ACTION_SERVE_FILE:
        serveFile();
        break;
        case ACTION_SERVE_INDEX:
        serveFile();
        break;
        case ACTION_AUTOINDEX:
        generateAutoIndex();
        break;
        case ACTION_REDIRECT:
        makeRedirect();
        break;
        default:
        makeError(result.statusCode);
        break;
    }
}

void HttpRequestHandler::handleDelete()
{
    const RouteResult& result = request._routeResult;
    const std::string& filePath = result.targetPath;
    struct stat st;

    if (stat(filePath.c_str(), &st) != 0)
    {
        makeError(404);
        return;
    }
    if (!S_ISREG(st.st_mode))
    {
        makeError(403);
        return;
    }
    if (remove(filePath.c_str()) != 0)//it will remove only if it was a file and not a directory
    {
        makeError(500);
        return;
    }
    response.setStatusCode(200);
    response.setStatusMessage("OK");
    response.setBodySource(BODY_NONE);
    response.setHeader("Content-Length", "0");
    response.setHeader("Connection", "keep-alive");
    response.setHeader("Date", getCurrentDate());
    response.setHeader("server", server_name);
}

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