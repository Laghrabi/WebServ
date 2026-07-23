#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

# include <iostream>
# include <vector>
# include <map>
# include <string>
# include <sstream>
# include <fstream>
# include <algorithm>
# include <ctime>
# include <cstdio>
#include <cctype>
#include "webserver.hpp"


/**
 * @enum ParseState
 * @brief Represents the states of the HTTP request parsing finite state machine.
 * 
 * This enum tracks the current progress of the parser. It allows the parser to 
 * pause and resume if a request is received in multiple chunks over the network.
 */
enum ParseState {
    READING_REQUEST_LINE,
    READING_HEADERS,
    READING_BODY,
    READING_CHUNK_SIZE,
    READING_CHUNK_DATA,
    READING_TRAILERS,
    FINISHED,
	ERROR
};

/**
 * @brief Defines standard HTTP status codes used throughout the server.
 * Maps specific error and success states encountered during request parsing 
 * and response generation to their corresponding RFC 7231 integer codes. 
 * This centralizes status management and prevents the use of magic numbers.
 */
enum HttpStatus {
    OK = 200,
    MOVED_PERMANENTLY = 301,
    FOUND = 302,
    TEMPORARY_REDIRECT = 307,
    BAD_REQUEST = 400,
    FORBIDDEN = 403,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    BODY_LENGTH_REQUIRED = 411,
    PAYLOAD_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    HTTP_VERSION_NOT_SUPPORTED = 505
};

class HttpRequest {
    private:
        HttpStatus							_statusCode;
        ParseState                      	_currentState;
        std::vector<char>	                _savedData;
        size_t				                _bufferIndex;          
        std::string							_method;
        std::string							_uri; // uri string with querries not narmalized and not decoded.
        std::string							_routeUri; // uri string without querries normalized and decoded.
        std::string                         _EncodedRouteUri; // uri string without querries normalized and not decoded.
        std::vector<std::string>            _EncodedUriSegments; // vector of encoded and normalized uri segments.
        std::vector<std::string>            _UriSegments; // vector of decoded and normalized uri segments.
    	std::string							_queryString;
        std::multimap<std::string, std::string> _queryParams;
        std::string							_version;
        std::map<std::string, std::string>	_headers;
        size_t                              _contentLength;
        size_t                              _chunkedSize;
        std::vector<char>                   _body;
        size_t								_bodyBytesWritten;
        static const size_t                 _MAX_BODY_SIZE = 10485760;
        static const size_t                 _DEFAULT_BODY_SIZE = 1048576;
        size_t                              _client_max_body_size;
        const Server*                       _server;
        Config::ServerRange                 _serverRange;
        RouteResult                         _routeResult;
        std::ofstream _bodyStream;
        std::string _bodyFilePath;
        Server::IPort                       _clientEndPoint;

        bool	parseRequestLine();
		bool	parseHeaders();
        bool    validateHeaders();
		bool	parseBody();
        bool    parseChunkSize();
        bool    parseChunkData();
		bool	validateMethod();
		bool	validateVersion();
        bool	decodeString(std::string& target);
        bool    uriDecode();
		bool	splitQueryString();
        bool    parseQueryParams();
        bool    normalizeUri();
        const Server* findServer(const std::string& name);
        void tokenizeUri(std::vector<std::string>& segments) const;
		
        
        public:
        static bool    normalizeUriHelper(std::string& uri,std::vector<std::string>& stack);
        HttpRequest();
        HttpRequest(const Config::ServerRange& serverRange, const Server::IPort& clientEndPoint);
        HttpRequest(const HttpRequest& other);
        HttpRequest& operator=(const HttpRequest& other);
        ~HttpRequest();
    
        void	parse(const std::vector<char>& rawBuffer);
        void    reset();

        static void printHttpStatus(HttpStatus status);

        std::vector<char> getLeftoverData() const;
        const std::string& getMethod() const;
        const std::string& getUri() const;
        const std::string& getVersion() const;
        const std::vector<char>& getBody() const;
        const std::map<std::string, std::string>& getHeaders() const;
		const std::string& getRouteUri() const;
    	const std::string& getQueryString() const;
        std::string getHeader(const std::string& key) const;
        ParseState getCurrentState() const;
		HttpStatus	getStatusCode() const;
        const Server* getServer() const;
        const std::multimap<std::string, std::string>& getQueryParams() const;
        const std::vector<std::string>& getUriSegments() const;
        const std::string& getEncodedUri() const;
        const std::vector<std::string>& getEncodedUriSegments() const;
				const Server::IPort& getServerIPort() const;
				const Server::IPort& getClientIPort() const;
};

char	safeToLower(char c);

#endif
