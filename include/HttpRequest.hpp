#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP

#include "webserver.hpp"
#include "RouteResult.hpp"


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
        std::string                         _host;
        std::multimap<std::string, std::string> _queryParams;
        std::string							_version;
        std::map<std::string, std::string>	_headers;
        size_t                              _contentLength;
        size_t                              _chunkedSize;
        std::vector<char>                   _body;
        const Server*                       _server;
        Config::ServerRange                 _serverRange;
        std::ofstream                       _bodyStream;
        std::string                         _bodyFilePath;
        Server::IPort                       _clientEndPoint;
        bool                                _has_max_body_size;
        size_t                              _client_max_body_size;

        HttpRequest();
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
        std::string generateSecureFileName(const std::string& directory, const std::string& extension);
        bool openBodyStream();
        
        public:
        size_t								_bodyBytesWritten;
        RouteResult                         _routeResult;

        void clearBodyFilePath();

        HttpRequest(const Config::ServerRange& serverRange, const Server::IPort& clientEndPoint);
        HttpRequest(const HttpRequest& other);
        HttpRequest& operator=(const HttpRequest& other);
        ~HttpRequest();
        
        void	parse(const std::vector<char>& rawBuffer);
        void    reset();
        
        static bool    normalizeUriHelper(std::string& uri,std::vector<std::string>& stack);
        static void printHttpStatus(HttpStatus status);
        
        const Config::ServerRange& getServerRange() const;
        std::vector<char> getLeftoverData() const;
        const std::string& getMethod() const;
        const std::string& getUri() const;
        const std::string& getVersion() const;
        const std::vector<char>& getBody() const;
        const std::map<std::string, std::string>& getHeaders() const;
		const std::string& getRouteUri() const;
        const std::string& getHost() const;
    	const std::string& getQueryString() const;
        std::string getHeader(const std::string& key) const;
        ParseState getCurrentState() const;
		HttpStatus	getStatusCode() const;
        const Server* getServer() const;
        const std::multimap<std::string, std::string>& getQueryParams() const;
        const std::vector<std::string>& getUriSegments() const;
        const std::string& getEncodedUri() const;
        const std::vector<std::string>& getEncodedUriSegments() const;
        const std::string& getBodyFilePath() const;
        const std::ofstream& getBodyStream() const;
        void printBodyContent() const;
        void debugPrintHeaders() const;
		const Server::IPort& getServerIPort() const;
		const Server::IPort& getClientIPort() const;

			void removeTmpFile(void);
};

char	safeToLower(char c);

#endif
