/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:13:03 by claghrab          #+#    #+#             */
/*   Updated: 2026/06/21 16:39:58 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
 *
enum HttpStatus {
    OK = 200,
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    METHOD_NOT_ALLOWED = 405,
    PAYLOAD_TOO_LARGE = 413,
    URI_TOO_LONG = 414,
    INTERNAL_SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501,
    HTTP_VERSION_NOT_SUPPORTED = 505
};

class HttpRequest {
    private:
        ParseState	_currentState;

        // The permanent storage and its bookmark
        std::vector<char>	_savedData;
        size_t				_bufferIndex;          

        // Storage for the parsed data
        int									_statusCode;
        std::string							_method;
        std::string							_uri;
        std::string							_version;
        std::map<std::string, std::string>	_headers;
        size_t                              _contentLength;
        size_t                              _chunkedSize;
        std::vector<char>                   _body;
        size_t								_bodyBytesWritten;
        static const size_t                 _MAX_BODY_SIZE = 10485760;

        bool	parseRequestLine();
		bool	parseHeaders();
        bool    validateHeaders();
		bool	parseBody();
        bool    parseChunkSize();
        bool    parseChunkData();
		bool	validateMethod();
		bool	validateVersion();
        bool	uriDecode();
		
        HttpRequest(const HttpRequest& other);
        HttpRequest& operator=(const HttpRequest& other);
        
    public:
        HttpRequest();
        ~HttpRequest();
    
        void	parse(const std::vector<char>& rawBuffer);

        void appendData(const char* data, size_t length); // for testing

        std::vector<char> getLeftoverData() const;
        const std::string& getMethod() const;
        const std::string& getUri() const;
        const std::string& getVersion() const;
        const std::vector<char>& getBody() const;
        const std::map<std::string, std::string>& getHeaders() const;
        std::string getHeader(const std::string& key) const;
        ParseState getCurrentState() const;
		int	getStatusCode() const;
};

char	safeToLower(char c);

#endif