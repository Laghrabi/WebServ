
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
		
		std::vector<char> assembleResponse() const;

		void clear();

		void setStatusCode(int code);
		void setStatusMessage(const std::string& message);
		void setHeader(const std::string& key, const std::string& value);
		void setBodySource(ResponseBodySource source);
		void setFilePath(const std::string& path);
		void setBufferBody(const std::vector<char>& body);
		void setContentLength(size_t length);
		void setHeadersSent(bool sent);
		void setByteSent(size_t bytes);

		bool getHeadersSent() const;
		ResponseBodySource getBodySource() const;
		const std::string& getFilePath() const;
		const std::vector<char>& getBufferBody() const;
		size_t getContentLength() const;
		size_t getBytesSent() const;
	};

#endif