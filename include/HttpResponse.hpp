
#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP


// #include "webserver.hpp"
#include "HttpRequest.hpp"

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
    BODY_FILE,
		BODY_PIPE
};

class HttpResponse
{
	private:
		int 										bufferBytesSent;
		ResponseBodySource							bodySource;
		std::string									filePath;
		std::ifstream								fileBody;
	    size_t										contentLength;
		size_t										filebytesSent;
		bool 										headersSent;
		static std::map<HttpStatus, std::string> 	statusCodeMap;
		HttpResponse& operator=(const HttpResponse& other);
		
	public:
		const Config&								config;
		int keep_connection;
		std::vector<char>							buffer;
		static void init();
		HttpResponse(const Config& config);
		HttpResponse(const HttpResponse& other);
		~HttpResponse();
		
		std::vector<char> assembleResponse();
		bool is_finished;
		bool is_ok_send;

		void clear();

		static std::string getCurrentDate();
		void setHeader(const std::string& key, const std::string& value, std::vector<char>& buffer);
		void setBodySource(ResponseBodySource source);
		void setFilePath(const std::string& path);
		void setFileBody(const std::string& path);
		void setContentLength(size_t length);
		void setHeadersSent(bool sent);
		void eraseSendBytes(size_t bytes);

		void setbufferBytesSent(int bytes)
		{
			bufferBytesSent += bytes;
		}

		bool getHeadersSent() const;
		int getBufferBytesSent() const;
		ResponseBodySource getBodySource() const;
		const std::string& getFilePath() const;
		const std::vector<char> getBufferBody() const;
		size_t getContentLength() const;
		size_t getBytesSent() const;
		void makeErrorCgi(HttpStatus code, const HttpRequest& reques);
		const std::map<HttpStatus, std::string>&  getStatusCodeMap();

	};



#endif
