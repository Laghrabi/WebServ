#include "HttpRequestHandler.hpp"
#include "HttpRequest.hpp"
#include "RouteResult.hpp"
#include "MimeTypesExt.hpp"


// i have to check if the client send connection close
// and if so, i have to disconect the clien ffuuuuuuuck
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
	std::cerr << "SERVE FILEones" << std::endl;
	std::vector<char>& buffer = response.buffer;
	std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + " OK\r\n";
	response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());
	response.setHeader("Content-Length", to_string(st.st_size), buffer);
	// response.setHeader("Content-Type", MimeTypesExt::getMimeType(filePath), buffer);//;ochkile hna ma3rftoch
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
    if (!p)
    {
        makeError(INTERNAL_SERVER_ERROR);
    }
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

    std::map<HttpStatus, std::string>::const_iterator it = response.getStatusCodeMap().find(request.getStatusCode());
	std::string assemble = "HTTP/1.1 " + to_string(it->first) + " " + it->second + "\r\n";
	buffer.insert(buffer.end(), assemble.begin(), assemble.end());

	response.setBodySource(BODY_NONE);
	response.setHeader("Location", result.targetPath, buffer);
	response.setHeader("Content-Length", "0", buffer);
	response.setHeader("Connection", "keep-alive", buffer);
	response.setHeader("Date", getCurrentDate(), buffer);
	response.setHeader("server", SERVER_NAME, buffer);
    buffer.insert(buffer.end(), std::string("\r\n").begin(), std::string("\r\n").end());
}

void HttpRequestHandler::makeError(HttpStatus code)
{
	std::vector<char>& buffer = response.buffer;
	std::string assemble = "HTTP/1.1 " + to_string(code) +  " " + response.getStatusCodeMap().find(code)->second + "\r\n";
	response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());
	// response.setStatusMessage(getStatusMessage(code));//i will wait for my partner to implement the getStatusMessage map
	// here i should check if there is a custom error page for this code and if yes i should set the filebody to that page
	// if no i will creat i simple html error page with the code and the message
	response.setBodySource(BODY_NONE);
	if (code == METHOD_NOT_ALLOWED) //NEED TO KNOW IS IT FORBIDDEN METHODS OR JUST FORBIDDEN ACCESS TO THE RESOURCE
		response.setHeader("Allow", "GET", response.buffer);  //how can i know the allowed methods for this route?
	response.setHeader("Content-Length", "0", response.buffer);
	response.setHeader("Connection", "keep-alive", response.buffer);
	response.setHeader("Date", getCurrentDate(), response.buffer);
	response.setHeader("server", SERVER_NAME, response.buffer);
    //still have to add \r\n and the bodyhtml
}

void HttpRequestHandler::handleGet()
{
	const RouteResult &result = request._routeResult;

    if (!result.route->isAllowed("GET"))
    {
        makeError(METHOD_NOT_ALLOWED);
        return;
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
        case ACTION_REDIRECT:
            makeRedirect();
            break;
		default:;
	}
}

void HttpRequestHandler::handleDelete()
{
    const RouteResult &result = request._routeResult;
    const std::string &filePath = result.targetPath;
    struct stat st;

    if (!result.route->isAllowed("DELETE"))
    {
        makeError(METHOD_NOT_ALLOWED);
        return;
    }
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
		if (errno == EACCES)
			makeError(FORBIDDEN);//if the dir doesnt have a read or write perm or 
		else
			makeError(INTERNAL_SERVER_ERROR);  // 500
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

void HttpRequestHandler::handlePost()
{
    const RouteResult &result = request._routeResult;
    const std::string &filePath = result.targetPath;
    if (!result.route->isAllowed("POST"))
    {
        makeError(METHOD_NOT_ALLOWED);
        return;
    }

    struct stat st;
    int created = 201;
    if (stat(filePath.c_str(), &st) == 0)
    {
        if (!S_ISREG(st.st_mode))
        {
            makeError(CONFLICT);
            return;
        }
        if (access(filePath.c_str(), W_OK) != 0)
        {
            makeError(FORBIDDEN);
            return;
        }
        created = 200;
    }
    if (rename(request.getBodyFilePath().c_str(),
           result.targetPath.c_str()) != 0)
    {
		switch (errno)
		{
			case ENOENT:
				// Source file doesn't exist or destination directory doesn't exist.
				// For uploads, this usually means the target directory is missing.
				makeError(CONFLICT);
				break;

			case ENOSPC:
				// Disk is full.
				makeError(INSUFFICIENT_STORAGE);
				break;

			default:
				makeError(INTERNAL_SERVER_ERROR);
				break;
		}
    return;
    }
	//hadchi ghir tkhrbi9 sara7a
    // else
    // {
    //     std::ifstream src(request.getBodyFilePath().c_str(), std::ios::in | std::ios::binary);
    //     if (!src.is_open())
    //     {
    //         makeError(INTERNAL_SERVER_ERROR);
    //         return;
    //     }
    //     std::ofstream dst(result.targetPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
    //     if (!dst.is_open())
    //     {
    //         src.close();
    //         makeError(INTERNAL_SERVER_ERROR);
    //         return;
    //     }
    //     char buffer[4096];
    //     while (src.good())
    //     {
    //         src.read(buffer, sizeof(buffer));
    //         std::streamsize count = src.gcount();
    //         if (count > 0)
    //             dst.write(buffer, count);
    //     }
    //     if (!src.eof() || !dst.good())
    //     {
    //         src.close();
    //         dst.close();
    //         remove(result.targetPath.c_str());
    //         makeError(INTERNAL_SERVER_ERROR);
    //         return;
    //     }
    //     src.close();
    //     dst.close();
    //     remove(request.getBodyFilePath().c_str());
    // }
    std::vector<char> &bufferResponse = response.buffer;

	std::string assemble = "HTTP/1.1 " + to_string(created);
    if (created == 201)
        assemble += " Created\r\n";
    else
        assemble += " OK\r\n";
    bufferResponse.insert(bufferResponse.end(),
                        assemble.begin(), assemble.end());

    response.setBodySource(BODY_NONE);
    response.setHeader("Content-Length", "0", bufferResponse);
    response.setHeader("Connection", "keep-alive", bufferResponse);
    response.setHeader("Date", getCurrentDate(), bufferResponse);
    response.setHeader("server", SERVER_NAME, bufferResponse);

    std::string newline("\r\n");
    bufferResponse.insert(bufferResponse.end(),
                        newline.begin(), newline.end());
}

void HttpRequestHandler::handleRequest()
{

	if (request.getCurrentState() != FINISHED) {
		makeError(request.getStatusCode());
		return ;
	}
	if (request._routeResult.action == ACTION_ERROR) {
		std::cout << "i have error action" << request._routeResult.statusCode << std::endl;
		makeError(request._routeResult.statusCode);
        return;
	}
	if (request._routeResult.action == ACTION_EXECUTE_CGI)
		return;

	if (request.getMethod() == "GET")
	{
		std::cout << "i have get method"<< std::endl;
		handleGet();
	}
	else if (request.getMethod() == "DELETE")
	{
		handleDelete();
	}
	else if (request.getMethod() == "POST")
	{
		std::cout << "im gonna handle post" << std::endl;
	    handlePost();
	}
}
