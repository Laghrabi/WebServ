#include "HttpResponse.hpp"
#include "webserver.hpp"

std::map<HttpStatus, std::string> 	HttpResponse::statusCodeMap;

std::string HttpResponse::getCurrentDate()
{
	std::time_t now = std::time(NULL);
	char buf[100];
	std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", std::gmtime(&now));
	return std::string(buf);
}

HttpResponse::HttpResponse(const Config& Config):
	bufferBytesSent(0),
	bodySource(BODY_NONE),
	filePath(),
	contentLength(0),
	filebytesSent(0),
	headersSent(false),
	config(Config),
	buffer(),
	is_finished(false)
{
	buffer.reserve(SENDSIZE);
	init();
}

HttpResponse::HttpResponse(const HttpResponse& other):
	bufferBytesSent(other.bufferBytesSent),
	bodySource(other.bodySource),
	filePath(other.filePath),
	contentLength(other.contentLength),
	filebytesSent(other.filebytesSent),
	headersSent(other.headersSent),
	config(other.config),
	buffer(other.buffer),
	is_finished(other.is_finished)
{
	buffer.reserve(SENDSIZE);
	init();
}


HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
	if (this != &other) {
		bufferBytesSent = other.bufferBytesSent;
		bodySource = other.bodySource;
		filePath = other.filePath;
		buffer = other.buffer;
		contentLength = other.contentLength;
		filebytesSent = other.filebytesSent;
		headersSent = other.headersSent;
		buffer.reserve(SENDSIZE);
		is_finished = other.is_finished;
	}
	return *this;
}

HttpResponse::~HttpResponse() {
	if (fileBody.is_open()) {
		fileBody.close();
	}
}

void HttpResponse::setHeader(const std::string& key, const std::string& value, std::vector<char>& buffer)
{
	std::string headerLine = key + ": " + value + "\r\n";
	buffer.insert(buffer.end(), headerLine.begin(), headerLine.end());
}

void HttpResponse::setBodySource(ResponseBodySource source)
{
	bodySource = source;
}

void HttpResponse::setFilePath(const std::string& path)
{
	filePath = path;
}

void HttpResponse::setFileBody(const std::string& path)
{
	fileBody.open(path.c_str());
}

void HttpResponse::setContentLength(size_t length)
{
	contentLength = length;
}

void HttpResponse::setHeadersSent(bool sent)
{
	headersSent = sent;
}

void HttpResponse::eraseSendBytes(size_t bytes)
{
	std::vector<char>::iterator vecBegin = buffer.begin();
	buffer.erase(vecBegin, vecBegin + bytes);
}


bool HttpResponse::getHeadersSent() const
{
	return headersSent;
}

int HttpResponse::getBufferBytesSent() const
{
	return bufferBytesSent;
}

ResponseBodySource HttpResponse::getBodySource() const
{
	return bodySource;
}

const std::string& HttpResponse::getFilePath() const
{
	return filePath;
}

const std::vector<char> HttpResponse::getBufferBody() const
{
	return buffer;
}

size_t HttpResponse::getContentLength() const
{
	return contentLength;
}

size_t HttpResponse::getBytesSent() const
{
	return filebytesSent;
}


const std::map<HttpStatus, std::string>&  HttpResponse::getStatusCodeMap()
{
	return (statusCodeMap);
}

void print_state (const std::ios& stream) {
	std::cout << " good()=" << stream.good();
	std::cout << ", eof()=" << stream.eof();
	std::cout << ", fail()=" << stream.fail();
	std::cout << ", bad()=" << stream.bad();
	std::cout << std::endl;
}

void HttpResponse::clear()
{
	bufferBytesSent = 0;
	bodySource = BODY_NONE;
	if (fileBody.is_open()) {
		fileBody.close();
	}
	filePath.clear();
	buffer.clear();
	contentLength = 0;
	filebytesSent = 0;
	headersSent = false;
}

std::vector<char> HttpResponse::assembleResponse() {
	if (getBodySource() == BODY_PIPE) {
		return (buffer);
		// NOTE: check if buffer is 
	}
	if (!getHeadersSent()) {
		if (!buffer.empty()) {
			if (buffer.size() <= SENDSIZE)
				return std::vector<char>(buffer.begin(), buffer.end());
			return std::vector<char>(buffer.begin(), buffer.begin() + SENDSIZE);
		}
		else {
			setHeadersSent(true);
			buffer.clear();
		}
	}
	if (getBodySource() == BODY_FILE) {
		if (buffer.empty())
		{
			char buf[SENDSIZE + 1];
			buf[SENDSIZE] = 0;
			fileBody.read(buf, SENDSIZE);
			size_t size = fileBody.gcount();
			if (size > 0)
				copyArrayToVec(buf, size, buffer);
		}
	}

	if (buffer.empty()) {
		is_finished = true;
	}

	return buffer;
}

void HttpResponse::makeErrorCgi(HttpStatus code, HttpRequest& request)
{
	std::cout << "[CGI]: making error for the CGI" << std::endl;
	std::string assemble = "HTTP/1.1 " + to_string(code) +  " " + getStatusCodeMap().find(code)->second + "\r\n";
	buffer.insert(buffer.end(), assemble.begin(), assemble.end());
	setBodySource(BODY_BUFFER);
	std::string connection = request.getHeader("connection");
	keep_connection = 1;
	if (connection == "" || connection == "keep-alive")
	{
		connection = "keep-alive";
	}
	else if (connection == "close")
	{
		keep_connection = 0;
	}
	setHeader("Connection", connection, buffer);
	setHeader("Date", HttpResponse::getCurrentDate(), buffer);
	setHeader("server", SERVER_NAME, buffer);
	std::string newline("\r\n");
	buffer.insert(buffer.end(), newline.begin(), newline.end());
}

void HttpResponse::init()
{
	if (!statusCodeMap.empty())
		return;

	// 2xx Success
	statusCodeMap.insert(std::make_pair(OK, "OK"));
	statusCodeMap.insert(std::make_pair(CREATED, "Created"));
	statusCodeMap.insert(std::make_pair(ACCEPTED, "Accepted"));
	statusCodeMap.insert(std::make_pair(NO_CONTENT, "No Content"));

	// 3xx Redirection
	statusCodeMap.insert(std::make_pair(MULTIPLE_CHOICES, "Multiple Choices"));
	statusCodeMap.insert(std::make_pair(MOVED_PERMANENTLY, "Moved Permanently"));
	statusCodeMap.insert(std::make_pair(FOUND, "Found"));
	statusCodeMap.insert(std::make_pair(SEE_OTHER, "See Other"));
	statusCodeMap.insert(std::make_pair(NOT_MODIFIED, "Not Modified"));
	statusCodeMap.insert(std::make_pair(USE_PROXY, "Use Proxy"));
	statusCodeMap.insert(std::make_pair(UNUSED, "(Unused)"));
	statusCodeMap.insert(std::make_pair(TEMPORARY_REDIRECT, "Temporary Redirect"));
	statusCodeMap.insert(std::make_pair(PERMANENT_REDIRECT, "Permanent Redirect"));

	// 4xx Client Errors
	statusCodeMap.insert(std::make_pair(BAD_REQUEST, "Bad Request"));
	statusCodeMap.insert(std::make_pair(FORBIDDEN, "Forbidden"));
	statusCodeMap.insert(std::make_pair(NOT_FOUND, "Not Found"));
	statusCodeMap.insert(std::make_pair(METHOD_NOT_ALLOWED, "Method Not Allowed"));
	statusCodeMap.insert(std::make_pair(REQUEST_TIMEOUT, "Request Timeout"));
	statusCodeMap.insert(std::make_pair(CONFLICT, "Conflict"));
	statusCodeMap.insert(std::make_pair(BODY_LENGTH_REQUIRED, "Length Required"));
	statusCodeMap.insert(std::make_pair(PAYLOAD_TOO_LARGE, "Payload Too Large"));
	statusCodeMap.insert(std::make_pair(URI_TOO_LONG, "URI Too Long"));
	statusCodeMap.insert(std::make_pair(UNSUPPORTED_MEDIA_TYPE, "Unsupported Media Type"));

	// 5xx Server Errors
	statusCodeMap.insert(std::make_pair(INTERNAL_SERVER_ERROR, "Internal Server Error"));
	statusCodeMap.insert(std::make_pair(NOT_IMPLEMENTED, "Not Implemented"));
	statusCodeMap.insert(std::make_pair(BAD_GATEWAY, "Bad Gateway"));
	statusCodeMap.insert(std::make_pair(SERVICE_UNAVAILABLE, "Service Unavailable"));
	statusCodeMap.insert(std::make_pair(GATEWAY_TIMEOUT, "Gateway Timeout"));
	statusCodeMap.insert(std::make_pair(HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported"));
}
