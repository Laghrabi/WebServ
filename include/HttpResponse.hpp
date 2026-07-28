
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "webserver.hpp"


// // headers that i need to handle
// // Content-Length
// // Content-Type
// // Date ----> Date: Tue, 25 Jul 2026 18:31:20 GMT
// // Server
// // Connection---->keep alive or close
// // location in case of rediraction
// //             HTTP/1.1 301 Moved Permanently
// //             Location: /new-page
// // Allow
// //     405 Method Not Allowed
// //     Allow: GET, POST

#define SENDSIZE 4096

enum ResponseBodySource
{
	BODY_NONE,
    BODY_BUFFER,
    BODY_FILE
};

class HttpResponse
{
	private:
		std::string							httpVersion;
		int									statusCode;
		std::string							statusMessage;
		std::map<std::string, std::string>	headers;
		ResponseBodySource					bodySource;
		std::ifstream						fileBody;
		std::string							filePath;
		std::vector<char>					bufferBody;
	    size_t								contentLength;
		size_t								bytesSent;
		bool 								headersSent;

	public:
		HttpResponse();
		HttpResponse(const HttpResponse& other);
		HttpResponse& operator=(const HttpResponse& other);
		~HttpResponse();
		
		std::vector<char> assembleResponse() const {
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

		void setStatusCode(int code);
		void setStatusMessage(const std::string& message);
		void setHeader(const std::string& key, const std::string& value);
		void setBodySource(ResponseBodySource source);
		void setFilePath(const std::string& path);
		void setBufferBody(const std::vector<char>& body);
		void setContentLength(size_t length);
		void setHeadersSent(bool sent);
		void setByteSent(size_t bytes);
	};

#endif