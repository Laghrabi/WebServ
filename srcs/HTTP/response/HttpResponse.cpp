#include "webserver.hpp"


	HttpResponse::HttpResponse() : httpVersion("HTTP/1.1") {}
	
	HttpResponse::HttpResponse(const HttpResponse& other): httpVersion(other.httpVersion), statusCode(other.statusCode),
		statusMessage(other.statusMessage), headers(other.headers), bodySource(other.bodySource),
		filePath(other.filePath), bufferBody(other.bufferBody), contentLength(other.contentLength),
		bytesSent(other.bytesSent), headersSent(other.headersSent)
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

	std::vector<char> HttpResponse::assembleResponse() const {
		std::vector<char> responseBuffer;
		std::string statusLine = httpVersion + " " + std::to_string(statusCode) + " " + statusMessage + "\r\n";
		responseBuffer.insert(responseBuffer.end(), statusLine.begin(), statusLine.end());
		for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
			std::string headerLine = it->first + ": " + it->second + "\r\n";
			responseBuffer.insert(responseBuffer.end(), headerLine.begin(), headerLine.end());
		}
		responseBuffer.insert(responseBuffer.end(), '\r');
		responseBuffer.insert(responseBuffer.end(), '\n');
		if (bodySource == BODY_BUFFER) {
			responseBuffer.insert(responseBuffer.end(), bufferBody.begin(), bufferBody.end());
		}
		return responseBuffer;
	}
