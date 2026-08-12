#include "HttpRequestHandler.hpp"
#include "HttpRequest.hpp"
#include "RouteResult.hpp"
#include "MimeTypesExt.hpp"


HttpRequestHandler::HttpRequestHandler(HttpRequest& req, HttpResponse& res, Session* ses) : request(req), response(res), session(ses) {}


HttpRequestHandler::~HttpRequestHandler()
{}

HttpRequestHandler::HttpRequestHandler(const HttpRequestHandler& other):
request(other.request), response(other.response), session(other.session)
{
	(void) other;
}
HttpRequestHandler& HttpRequestHandler::operator=(const HttpRequestHandler& other) {
	(void) other;
	return *this;
}

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

void HttpRequestHandler::standardHeader(std::vector<char>& buffer , std::string connection)
{
	if (session)
	{
		std::string session_id = "session_id=" + session->getSessionId(); //+ "; Path=/";
		response.setHeader("Set-Cookie", session_id, buffer);
	}
	response.setHeader("Connection", connection, buffer);
	response.setHeader("Date", HttpResponse::getCurrentDate(), buffer);
	response.setHeader("server", SERVER_NAME, buffer);
	std::string newline("\r\n");
	buffer.insert(buffer.end(), newline.begin(), newline.end());
}

void HttpRequestHandler::serveFile()
{
	std::cout << "[RESPONSE]: serving file" << std::endl;
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
	std::string connection = checkConnection();
	std::vector<char>& buffer = response.buffer;
	std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + " " + response.getStatusCodeMap().find(result.statusCode)->second + "\r\n";
	response.last_code = result.statusCode;
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
		return "";

	struct dirent* l;
	std::string html;
	std::string parent = request.getRouteUri();

	while ((l = readdir(p))) {
		html += "<a href=\"" + parent + "/" + std::string(l->d_name) + "\">" + std::string(l->d_name) + "</a><br>\n";
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
	std::cout << "[RESPONSE]: generating autoindexing" << std::endl;
	std::string connection = checkConnection();
	std::string autoIndexHtml = generateAutoIndexHtml(directoryPath);
	if (autoIndexHtml == "")
	{
		makeError(FORBIDDEN);
		return;
	}
	std::vector<char>& buffer = response.buffer;
	std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + " " + response.getStatusCodeMap().find(result.statusCode)->second + "\r\n";
	response.last_code = result.statusCode;
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

	std::cout << "[RESPONSE]: make rediraction" << std::endl;
    std::map<HttpStatus, std::string>::const_iterator it = response.getStatusCodeMap().find(request._routeResult.statusCode);
	std::string assemble = "HTTP/1.1 " + to_string(it->first) + " " + it->second + "\r\n";
	response.last_code =  it->first;
	buffer.insert(buffer.end(), assemble.begin(), assemble.end());
	std::string errorHtml = request.getServer()->getErrorPage(response.last_code);
	FileStatus file(errorHtml);
	if (errorHtml == "" || file.isDir() || !file.exist())
	{
		errorHtml = generateErrorPage(response.last_code);
		response.setBodySource(BODY_BUFFER);
	}
	else {
		response.setBodySource(BODY_FILE);
		response.fileBody.open(errorHtml.c_str());
	}
	std::string connection = checkConnection();
	response.setHeader("Location", result.targetPath, buffer);
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
	response.is_ok_send = true;
	std::cout << "[RESPONSE]: making error" << std::endl;
	std::vector<char>& buffer = response.buffer;
	std::string assemble = "HTTP/1.1 " + to_string(code) +  " " + response.getStatusCodeMap().find(code)->second + "\r\n";
	buffer.insert(buffer.end(), assemble.begin(), assemble.end()); 
	response.last_code =  code;
	std::string errorHtml;
	const Server *server = request.getServer();
	if (server)
	{
		errorHtml = request.getServer()->getErrorPage(code);
		FileStatus file(errorHtml);
		if (errorHtml == "" || file.isDir() || !file.exist())
		{
			errorHtml = generateErrorPage(code);
			response.setBodySource(BODY_BUFFER);
		}
		else {
			response.setBodySource(BODY_FILE);
			response.fileBody.open(errorHtml.c_str());
		}
	}
	else {
		errorHtml = generateErrorPage(code);
		response.setBodySource(BODY_BUFFER);
	}

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

	std::cout << "[RESPONSE]: handle GIT" << std::endl; 
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
	std::cout << "[RESPONSE]: delete file " << filePath.c_str();
	std::string connection = checkConnection();
	std::vector<char>& buffer = response.buffer;
	std::string assemble = "HTTP/1.1 " + to_string(result.statusCode) + " " + response.getStatusCodeMap().find(result.statusCode)->second + "\r\n";
	response.last_code = result.statusCode;
	response.buffer.insert(buffer.end(), assemble.begin(), assemble.end());
	response.setBodySource(BODY_NONE);
	response.setHeader("Content-Length", "0", response.buffer);
	standardHeader(buffer, connection);
}

void HttpRequestHandler::handlePost()
{
    const RouteResult &result = request._routeResult;
	std::cout << "[RESPONSE]: post file" << std::endl;

	if (result.statusCode == CREATED || result.statusCode == OK)
	{
		request.clearBodyFilePath();
	}
    std::vector<char> &bufferResponse = response.buffer;
	std::string connection = checkConnection();
	std::string assemble = "HTTP/1.1 " ;
	assemble += to_string(result.statusCode)  + " " + response.getStatusCodeMap().find(result.statusCode)->second + "\r\n";
	response.last_code = result.statusCode;
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
