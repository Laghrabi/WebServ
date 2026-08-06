#include "HttpRequestHandler.hpp"
#include "HttpRequest.hpp"
#include "RouteResult.hpp"
#include "MimeTypesExt.hpp"

std::string HttpRequestHandler::checkConnection()
{
	std::string connection = request.getHeader("connection");
	response.keep_connection = 1;
	if (connection == "" || connection == "keep-alive")
	{
		connection = "keep-alive";
	}
	else if (connection == "close")
	{
		response.keep_connection = 0;
	}
	return connection;
}

<<<<<<< HEAD
void HttpRequestHandler::standardHeader(std::vector<char> buffer , std::string connection)
=======
void HttpRequestHandler::standardHeader(std::vector<char>& buffer , std::string connection)
>>>>>>> 93faf3f4fa76347df37a672b8e064c00d48a0aab
{
	response.setHeader("Connection", connection, buffer);
	response.setHeader("Date", HttpResponse::getCurrentDate(), buffer);
	response.setHeader("server", SERVER_NAME, buffer);
	std::string newline("\r\n");
<<<<<<< HEAD
	response.buffer.insert(buffer.end(), newline.begin(), newline.end());
=======
	buffer.insert(buffer.end(), newline.begin(), newline.end());
>>>>>>> 93faf3f4fa76347df37a672b8e064c00d48a0aab
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
	std::cout << "serving file" << std::endl;
	std::string connection = checkConnection();
	std::vector<char>& buffer = response.buffer;
	std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + " OK\r\n";
	response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());
	response.setHeader("Content-Length", to_string(st.st_size), buffer);
	response.setHeader("Content-Type", response.config.m_types.getMimeType(filePath), buffer);
	standardHeader(buffer, connection);

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
	const RouteResult &result = request._routeResult;
	const std::string &directoryPath = result.targetPath;
	if (access(directoryPath.c_str(), R_OK) != 0)
	{
		makeError(FORBIDDEN);
		return;
	}
	std::cout << "generating autoindexing" << std::endl;
	std::string connection = checkConnection();
	std::string autoIndexHtml = generateAutoIndexHtml(directoryPath);
	std::vector<char>& buffer = response.buffer;
	std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + " OK\r\n";
	response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());
	response.setHeader("Content-Type", response.config.m_types.getMimeType("dflk.html"), buffer);//hamza chof had l3iba
	response.setHeader("Content-Length", to_string(autoIndexHtml.size()), response.buffer);
	standardHeader(buffer, connection);
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

	std::string connection = checkConnection();
	std::cout << "make rediraction" << std::endl;
	response.setBodySource(BODY_NONE);
	response.setHeader("Location", result.targetPath, buffer);
	response.setHeader("Content-Length", "0", buffer);
	// response.setHeader("Content-Type", );
	standardHeader(buffer, connection);
}

std::string HttpRequestHandler::generateErrorPage(HttpStatus code)
{
    std::string message = response.getStatusCodeMap().find(code)->second;

    return "<!DOCTYPE html>\n"
           "<html>\n"
           "<head>\n"
           "    <title>" + to_string(code) + " " + message + "</title>\n"
           "</head>\n"
           "<body>\n"
           "    <h1>" + to_string(code) + " " + message + "</h1>\n"
           "    <hr>\n"
           "    <p>1337 Webserver</p>\n"
           "</body>\n"
           "</html>\n";
}

void HttpRequestHandler::makeError(HttpStatus code)
{
<<<<<<< HEAD
=======
	response.is_ok_send = true;
>>>>>>> 93faf3f4fa76347df37a672b8e064c00d48a0aab
	std::cout << "making error" << std::endl;
	std::vector<char>& buffer = response.buffer;
	std::string assemble = "HTTP/1.1 " + to_string(code) +  " " + response.getStatusCodeMap().find(code)->second + "\r\n";
	buffer.insert(buffer.end(), assemble.begin(), assemble.end());
	// here i should check if there is a custom error page for this code and if yes i should set the filebody to that page
	// if no i will creat i simple html error page with the code and the message
	
	response.setBodySource(BODY_BUFFER);
	std::string errorHtml = generateErrorPage(code);

	response.setHeader("Content-Length", to_string(errorHtml.size()), buffer);
	response.setHeader("Content-Type", response.config.m_types.getMimeType("dflk.html"), buffer);
	std::string connection = checkConnection();
	if (code == METHOD_NOT_ALLOWED)
	{
		const RouteConfig *config = request._routeResult.route;
		std::set<std::string> allowed_methods = config->getAllowedMethods();
		std::string methods;
		for (std::set<std::string>::const_iterator it = allowed_methods.begin();
			it != allowed_methods.end();
			++it)
		{
			if (!methods.empty())
				methods += ", ";
			methods += *it;
		}
		response.setHeader("Allow", methods, buffer);
	}
	standardHeader(buffer, connection);
	response.buffer.insert(buffer.end(), errorHtml.begin(), errorHtml.end());
}

void HttpRequestHandler::handleGet()
{
	const RouteResult &result = request._routeResult;

    if (!result.route->isAllowed("GET"))
    {
        makeError(METHOD_NOT_ALLOWED);
        return;
    }

	std::cout << "im gonna handle GIT" << std::endl; 
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
	std::cout << "[DELETE]: delete file " << filePath.c_str();
	std::string connection = checkConnection();
	std::vector<char>& buffer = response.buffer;
	std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + " OK\r\n";
	response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());
	response.setBodySource(BODY_NONE);
	response.setHeader("Content-Length", "0", response.buffer);
	standardHeader(buffer, connection);
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
	std::cout << "trying to handle post" << std::endl;
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
    std::vector<char> &bufferResponse = response.buffer;
	std::string connection = checkConnection();
	std::string assemble = "HTTP/1.1 " + to_string(created);
    if (created == 201)
        assemble += " Created\r\n";
    else
        assemble += " OK\r\n";
    bufferResponse.insert(bufferResponse.end(),
                        assemble.begin(), assemble.end());

    response.setBodySource(BODY_NONE);
    response.setHeader("Content-Length", "0", bufferResponse);
	standardHeader(bufferResponse, connection);
}

void HttpRequestHandler::handleRequest()
{

	if (request.getCurrentState() != FINISHED) {
		makeError(request.getStatusCode());
		return ;
	}
	if (request._routeResult.action == ACTION_ERROR) {
		makeError(request._routeResult.statusCode);
        return;
	}
	if (request._routeResult.action == ACTION_EXECUTE_CGI) {
		response.setBodySource(BODY_PIPE);
		return;
	}
	response.is_ok_send = true;
	if (request.getMethod() == "GET")
	{
		handleGet();
	}
	else if (request.getMethod() == "DELETE")
	{
		handleDelete();
	}
	else if (request.getMethod() == "POST")
	{
	    handlePost();
	}
}
