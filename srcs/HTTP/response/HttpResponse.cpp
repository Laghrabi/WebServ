#include "webserver.hpp"


	HttpResponse::HttpResponse() : httpVersion("HTTP/1.1"), bytesSent(0), headersSent(false) {}
	
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
    
    bool HttpResponse::getHeadersSent() const
    {
        return headersSent;
    }

    ResponseBodySource HttpResponse::getBodySource() const
    {
        return bodySource;
    }

    const std::string& HttpResponse::getFilePath() const
    {
        return filePath;
    }

    const std::vector<char>& HttpResponse::getBufferBody() const
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

    void HttpResponse::clear()
    {
        statusCode = 0;
        statusMessage.clear();
        headers.clear();
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
		return responseBuffer;
	}
