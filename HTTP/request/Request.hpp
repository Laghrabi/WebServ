/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:13:03 by claghrab          #+#    #+#             */
/*   Updated: 2026/05/12 14:59:04 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUEST_HPP
#define REQUEST_HPP

# include <iostream>
# include <vector>
# include <map>
# include <string>


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
    FINISHED
};

class HttpRequest {
    private:
        ParseState _currentState;

        // The permanent storage and its bookmark
        std::vector<char> _savedData;
        size_t _bufferIndex;          

        // Storage for the parsed data
        std::string _method;
        std::string _uri;
        std::string _version;
        std::map<std::string, std::string> _headers;
        std::vector<char> _body;

    public:
        HttpRequest();
        HttpRequest(const HttpRequest& other);
        HttpRequest& operator=(const HttpRequest& other);
        ~HttpRequest();
    
        void parse(const std::vector<char>& rawBuffer); 
};

#endif