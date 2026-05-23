/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:13:03 by claghrab          #+#    #+#             */
/*   Updated: 2026/05/22 17:56:45 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <iostream>
# include <vector>
# include <map>
# include <string>
# include <sstream>
# include <fstream>
# include <algorithm>
# include <ctime>
# include <cstdio>


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
    //READING_CHUNK_SIZE,
    //READING_CHUNK_DATA,
    FINISHED,
	ERROR
};

class HttpRequest {
    private:
        ParseState	_currentState;

        // The permanent storage and its bookmark
        std::vector<char>	_savedData;
        size_t				_bufferIndex;          

        // Storage for the parsed data
        std::string							_method;
        std::string							_uri;
        std::string							_version;
        std::map<std::string, std::string>	_headers;
        size_t                              _contentLength;
        std::ofstream                        _bodyStream;
        std::string                         _bodyFilePath;
        static int                          _fileCounter;
        size_t								_bodyBytesWritten;

        bool	parseRequestLine();
		bool	parseHeaders();
		bool	parseBody();
		
        HttpRequest(const HttpRequest& other) ;
        HttpRequest& operator=(const HttpRequest& other);
        
    public:
        HttpRequest();
        ~HttpRequest();
    
        void	parse(const std::vector<char>& rawBuffer);
        void    cleanupTempFile();

        std::vector<char> getLeftoverData() const;
        const std::string& getMethod() const;
        const std::string& getUri() const;
        const std::string& getVersion() const;
        const std::map<std::string, std::string>& getHeaders() const;
        std::string getHeader(const std::string& key) const;
        const std::string& getBodyFilePath() const;
        ParseState getCurrentState() const;
};

#endif