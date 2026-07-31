#include "HttpRequestHandler.hpp"

static std::string getCurrentDate()
{
    std::time_t now = std::time(NULL);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
    return std::string(buf);
}

void HttpRequestHandler::serveFile()
{
    const RouteResult &result = request._routeResult;
    const std::string &filePath = result.targetPath;
    struct stat st;
    stat(filePath.c_str(), &st);

    if (stat(filePath.c_str(), &st) == -1)
    {
        makeError(NOT_FOUND);
        return;
    }
    if (!S_ISREG(st.st_mode))
    {
        makeError(FORBIDDEN);
        return;
    }
    if (access(filePath.c_str(), R_OK) != 0)
    {
        makeError(FORBIDDEN);
        return;
    }
    std::vector<char>& buffer = response.buffer;
    std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + " OK\r\n";
    response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());
    response.setHeader("Content-Length", to_string(st.st_size), buffer);
    // response.setHeader("Content-Type", getMimeType(filePath), buffer);//wtabnasba lhadi ana gha guessit kighatkon
    response.setHeader("Connection", "keep-alive", buffer);
    response.setHeader("Date", getCurrentDate(), buffer);
    response.setHeader("server", SERVER_NAME, buffer);
    std::string newline("\r\n");
    response.buffer.insert(buffer.end(), newline.begin(), newline.end());

    response.setBodySource(BODY_FILE);
    response.setFilePath(filePath);
    response.setFileBody(filePath);
    response.setContentLength(st.st_size);
}

std::string HttpRequestHandler::generateAutoIndexHtml(const std::string &directoryPath)
{
        DIR* p = opendir(directoryPath.c_str());
        struct dirent* l;
        std::string html;
        while ((l = readdir(p))) {
                html += "<a href=\"" + std::string(l->d_name) + "\">" + std::string(l->d_name) + "</a><br>\n";
        }
        closedir(p);
        return html;
}

void HttpRequestHandler::generateAutoIndex()
{
    std::cerr <<"error\nA A" << "\n";
    const RouteResult &result = request._routeResult;
    const std::string &directoryPath = result.targetPath;
    if (access(directoryPath.c_str(), R_OK) != 0)
    {
        makeError(FORBIDDEN);
        return;
    }
    std::string autoIndexHtml = generateAutoIndexHtml(directoryPath);
    std::vector<char>& buffer = response.buffer;
    std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + " OK\r\n";
    response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());
    response.setHeader("Content-Type", "text/html", buffer);
    response.setHeader("Content-Length", to_string(autoIndexHtml.size()), response.buffer);
    response.setHeader("Connection", "keep-alive", buffer);
    response.setHeader("Date", getCurrentDate(), buffer);
    response.setHeader("server", SERVER_NAME, buffer);
    std::string newline("\r\n");
    response.buffer.insert(buffer.end(), newline.begin(), newline.end());
    response.buffer.insert(buffer.end(), autoIndexHtml.begin(), autoIndexHtml.end());
    response.setBodySource(BODY_BUFFER);
}

void HttpRequestHandler::makeRedirect()
{
    const RouteResult &result = request._routeResult;
    std::vector<char>& buffer = response.buffer;

    // std::map<HttpStatus, std::string>::iterator it = response.getStatusCodeMap().find(result.statusCode);
    // here in this line above chaimaa should replace the result.statusCode with the status code map not int
    // std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + it->second + "\r\n";
    // response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());

    response.setBodySource(BODY_NONE);
    response.setHeader("Location", result.targetPath, response.buffer);
    response.setHeader("Content-Length", "0", response.buffer);
    response.setHeader("Connection", "keep-alive", response.buffer);
    response.setHeader("Date", getCurrentDate(), response.buffer);
    response.setHeader("server", SERVER_NAME, response.buffer);
}

void HttpRequestHandler::makeError(HttpStatus code)
{
    std::vector<char>& buffer = response.buffer;
    std::string assemble = "HTTP/1.1 " + to_string(code) +  +"\r\n";
    response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());
    // response.setStatusMessage(getStatusMessage(code));//i will wait for my partner to implement the getStatusMessage map
    // here i should check if there is a custom error page for this code and if yes i should set the filebody to that page
    // if no i will creat i simple html error page with the code and the message
    response.setBodySource(BODY_NONE);
    if (code == FORBIDDEN) //NEED TO KNOW IS IT FORBIDDEN METHODS OR JUST FORBIDDEN ACCESS TO THE RESOURCE
        response.setHeader("Allow", "GET", response.buffer);  //how can i know the allowed methods for this route?
    response.setHeader("Content-Length", "0", response.buffer);
    response.setHeader("Connection", "keep-alive", response.buffer);
    response.setHeader("Date", getCurrentDate(), response.buffer);
    response.setHeader("server", SERVER_NAME, response.buffer);
}

void HttpRequestHandler::handleGet()
{
    const RouteResult &result = request._routeResult;
		int status_code;
		if (request.getCurrentState() != FINISHED) {
			status_code = request.getStatusCode();
		}

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
        // case ACTION_REDIRECT:
        //     makeRedirect();
        //     break;
        default:
            makeError(FORBIDDEN);//same here i will wait for chaimaa to replace it 
            break;
    }
}

void HttpRequestHandler::handleDelete()
{
    const RouteResult &result = request._routeResult;
    const std::string &filePath = result.targetPath;
    struct stat st;

    if (stat(filePath.c_str(), &st) != 0)
    {
        makeError(NOT_FOUND);
        return;
    }
    if (!S_ISREG(st.st_mode))
    {
        makeError(FORBIDDEN);
        return;
    }
    if (remove(filePath.c_str()) != 0) // it will remove only if it was a file and not a directory
    {
        //here  i would fail if it was a directory.
        // i dont know what should i do in this case. should i delete the directory and all its content or just return an error?
        makeError(INTERNAL_SERVER_ERROR);
        return;
    }

    std::vector<char>& buffer = response.buffer;
    std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + " OK\r\n";
    response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());
    response.setBodySource(BODY_NONE);
    response.setHeader("Content-Length", "0", response.buffer);
    response.setHeader("Connection", "keep-alive", response.buffer);
    response.setHeader("Date", getCurrentDate(), response.buffer);
    response.setHeader("server", SERVER_NAME, response.buffer);
}

void HttpRequestHandler::handleRequest()
{
    if (request.getMethod() == "GET")
    {
        handleGet();
    }
    else if (request.getMethod() == "DELETE")
    {
        handleDelete();
    }
    // else if (request.getMethod() == "POST")
    // {
    //     handlePost();
    // }
}
