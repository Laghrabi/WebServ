#include "webserver.hpp"
std::map<int, std::string> 	HttpResponse::statusCodeMap;

HttpResponse::HttpResponse()
	: httpVersion("HTTP/1.1"),
	  statusCode(),
	  statusMessage(),
	  headers(),
	  headersSize(0),
	  headersBytesSent(0),
	  bodySource(BODY_NONE),
	  filePath(),
	  bufferBody(),
	  contentLength(0),
	  bytesSent(0),
	  headersSent(false)
{
}

HttpResponse::HttpResponse(const HttpResponse& other)
	: httpVersion(other.httpVersion),
	  statusCode(other.statusCode),
	  statusMessage(other.statusMessage),
	  headers(other.headers),
	  headersSize(other.headersSize),
	  headersBytesSent(other.headersBytesSent),
	  bodySource(other.bodySource),
	  filePath(other.filePath),
	  bufferBody(other.bufferBody),
	  contentLength(other.contentLength),
	  bytesSent(other.bytesSent),
	  headersSent(other.headersSent)
{}


HttpResponse& HttpResponse::operator=(const HttpResponse& other) {
	if (this != &other) {
		httpVersion = other.httpVersion;
		statusCode = other.statusCode;
		statusMessage = other.statusMessage;
			headers = other.headers;
			bodySource = other.bodySource;
			filePath = other.filePath;
			bufferBody = other.bufferBody;
			contentLength = other.contentLength;
			bytesSent = other.bytesSent;
			headersSent = other.headersSent;
		}
		return *this;
}

HttpResponse::~HttpResponse() {
	if (fileBody.is_open()) {
		fileBody.close();
	}
}

void HttpResponse::setStatusCode(int code)
{
	statusCode = code;
}

void HttpResponse::setStatusMessage(const std::string& message)
{
	statusMessage = message;
}

void HttpResponse::setHeader(const std::string& key, const std::string& value)
{
	headers.insert(std::make_pair(key, value));
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
	std::cout << fileBody.is_open() << "yes" << std::endl;
	fileBody.open(path.c_str());
	std::cout << "len = " << path.length() << "\n";
	  std::cout << ", fail()=" << fileBody.fail() << "file name" << path << "bla" << std::endl;
}

void HttpResponse::setBufferBody(const std::vector<char>& body)
{
	bufferBody = body;
}

void HttpResponse::setContentLength(size_t length)
{
	contentLength = length;
}

void HttpResponse::setHeadersSent(bool sent)
{
	headersSent = sent;
}

void HttpResponse::setByteSent(size_t bytes)
{
	bytesSent += bytes;
}


bool HttpResponse::getHeadersSent() const
{
	return headersSent;
}

int HttpResponse::getHeadersSize() const
{
	return headersSize;
}

int HttpResponse::getHeadersBytesSent() const
{
	return headersBytesSent;
}

std::string HttpResponse::getStartline() const
{
	return httpVersion + " " + to_string(statusCode) + " " + statusMessage;
}

std::string HttpResponse::getHeaders() const
{
	std::string headersString;
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		headersString += it->first + ": " + it->second + "\r\n";
	}
	return headersString;
}

std::string HttpResponse::getStartlineAndHeaders() const
{
	std::string responseString = getStartline() + "\r\n" + getHeaders() + "\r\n";
	return responseString;
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
	return bufferBody;
}

size_t HttpResponse::getContentLength() const
{
	return contentLength;
}

size_t HttpResponse::getBytesSent() const
{
	return bytesSent;
}


const std::map<int, std::string>&  HttpResponse::getStatusCodeMap()
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
	statusCode = 0;
	statusMessage.clear();
	headers.clear();
	headersSize = 0;
	headersBytesSent = 0;
	bodySource = BODY_NONE;
	if (fileBody.is_open()) {
		fileBody.close();
	}
	filePath.clear();
	bufferBody.clear();
	contentLength = 0;
	bytesSent = 0;
	headersSent = false;
}

std::vector<char> HttpResponse::assembleResponse() {
	if (!getHeadersSent()) {
		std::string responseString = getStartlineAndHeaders();
		setHeadersSize(responseString.size());
		if (getHeadersBytesSent() < getHeadersSize()) {
			return std::vector<char>(responseString.begin() + getHeadersBytesSent(), responseString.end());
		}
		else
			setHeadersSent(true);
	}
	if (getBodySource() == BODY_BUFFER) {
		const std::vector<char> bodyBuffer = getBufferBody();
		size_t sent = getBytesSent();
		if (sent >= bodyBuffer.size())
		return std::vector<char>();
		size_t chunkSize = bodyBuffer.size() - sent;
		if (chunkSize > SENDSIZE)
		chunkSize = SENDSIZE;
		return std::vector<char>(bodyBuffer.begin() + sent,
		bodyBuffer.begin() + sent + chunkSize);
    } else if (getBodySource() == BODY_FILE) {
		std::cout << "hello\n";
        char buffer[4096] = {0};
			print_state(fileBody);
        fileBody.read(buffer, sizeof(buffer) - 1);
		print_state(fileBody);
		std::string str(buffer);
		
		// std::streamsize bytesRead = fileBody.gcount();
		std::cout << "-------------" << fileBody.good() << std::endl;
        // if (bytesRead <= 0)
        //     return std::vector<char>();
        return std::vector<char>(str.begin(), str.end());
    }
    return std::vector<char>();
}


void HttpResponse::init()
{
	if (!statusCodeMap.empty())
		return;

	// 2xx Success
	statusCodeMap.insert(std::make_pair(200, "OK"));
	statusCodeMap.insert(std::make_pair(201, "Created"));
	statusCodeMap.insert(std::make_pair(202, "Accepted"));
	statusCodeMap.insert(std::make_pair(204, "No Content"));

	// 3xx Redirection
	statusCodeMap.insert(std::make_pair(300, "Multiple Choices"));
	statusCodeMap.insert(std::make_pair(301, "Moved Permanently"));
	statusCodeMap.insert(std::make_pair(302, "Found"));
	statusCodeMap.insert(std::make_pair(303, "See Other"));
	statusCodeMap.insert(std::make_pair(304, "Not Modified"));
	statusCodeMap.insert(std::make_pair(305, "Use Proxy"));
	statusCodeMap.insert(std::make_pair(306, "(Unused)"));
	statusCodeMap.insert(std::make_pair(307, "Temporary Redirect"));
	statusCodeMap.insert(std::make_pair(308, "Permanent Redirect"));

	// 4xx Client Errors
	statusCodeMap.insert(std::make_pair(400, "Bad Request"));
	statusCodeMap.insert(std::make_pair(403, "Forbidden"));
	statusCodeMap.insert(std::make_pair(404, "Not Found"));
	statusCodeMap.insert(std::make_pair(405, "Method Not Allowed"));
	statusCodeMap.insert(std::make_pair(408, "Request Timeout"));
	statusCodeMap.insert(std::make_pair(409, "Conflict"));
	statusCodeMap.insert(std::make_pair(411, "Length Required"));
	statusCodeMap.insert(std::make_pair(413, "Payload Too Large"));
	statusCodeMap.insert(std::make_pair(414, "URI Too Long"));
	statusCodeMap.insert(std::make_pair(415, "Unsupported Media Type"));
	statusCodeMap.insert(std::make_pair(500, "Internal Server Error"));
	statusCodeMap.insert(std::make_pair(501, "Not Implemented"));
	statusCodeMap.insert(std::make_pair(502, "Bad Gateway"));
	statusCodeMap.insert(std::make_pair(503, "Service Unavailable"));
	statusCodeMap.insert(std::make_pair(504, "Gateway Timeout"));
	statusCodeMap.insert(std::make_pair(505, "HTTP Version Not Supported"));
}