#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "HttpRequest.hpp"

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
	    size_t										contentLength;
		size_t										filebytesSent;
		bool 										headersSent;
		static std::map<HttpStatus, std::string> 	statusCodeMap;
		
		public:
		HttpStatus									last_code;
		const Config&								config;
		int keep_connection;
		std::vector<char>							buffer;
		static void init();
		HttpResponse(const Config& config);
		HttpResponse(const HttpResponse& other);
		~HttpResponse();
		HttpResponse& operator=(const HttpResponse& other);
		
		std::ifstream								fileBody;
		size_t assembleResponse();
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

		void setbufferBytesSent(int bytes);

		bool getHeadersSent() const;
		int getBufferBytesSent() const;
		ResponseBodySource getBodySource() const;
		const std::string& getFilePath() const;
		const std::vector<char> getBufferBody() const;
		size_t getContentLength() const;
		size_t getBytesSent() const;
		void makeErrorCgi(HttpStatus code, const HttpRequest& reques);
		const std::map<HttpStatus, std::string>&  getStatusCodeMap();
		void setLog(const HttpRequest& request);
		std::string generateError(HttpStatus code);

	};



#endif
