/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:30:50 by claghrab          #+#    #+#             */
/*   Updated: 2026/05/14 18:13:57 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

/**
  * @brief Default constructor.
  * 
  * Initializes a new HTTP request, setting the initial parsing state 
  * to READING_REQUEST_LINE and the buffer index to 0.
  */
HttpRequest::HttpRequest() : _currentState(READING_REQUEST_LINE), _bufferIndex(0) {}

/**
  * @brief Copy constructor.
  * 
  * @param other The HttpRequest instance to copy from.
  */
HttpRequest::HttpRequest(const HttpRequest& other)
            : _currentState(other._currentState),
            _savedData(other._savedData),
            _bufferIndex(other._bufferIndex),
            _method(other._method),
            _uri(other._uri),
            _version(other._version),
            _headers(other._headers),
            _body(other._body) {}

/**
  * @brief Copy assignment operator.
  * 
  * @param other The HttpRequest instance to assign from.
  * @return A reference to the updated HttpRequest object.
  */
HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
    if (this != &other)
    {
        _currentState = other._currentState;
        _savedData = other._savedData;
        _bufferIndex = other._bufferIndex;
        _method = other._method;
        _uri = other._uri;
        _version = other._version;
        _headers = other._headers;
        _body = other._body;
    }
    return (*this);
}

/**
  * @brief Destructor.
  */
HttpRequest::~HttpRequest() {}

void HttpRequest::parse(const std::vector<char>& rawBuffer)
{
	if (rawBuffer.empty())
		return ;
	
	_savedData.insert(_savedData.end(), rawBuffer.begin(), rawBuffer.end());
	
	while (_bufferIndex < _savedData.size())
	{
		switch (_currentState)
		{
			case READING_REQUEST_LINE:
				parseRequestLine();
				break ;
			case READING_HEADERS:
				//TODO
				break ;
			case READING_BODY:
				//TODO
				break ;
			case FINISHED:
			case ERROR:
				return ;
		}
	}
}

void	HttpRequest::parseRequestLine()
{
	const std::string	crlf = "\r\n";

	auto it = std::search(
		_savedData.begin() + _bufferIndex, _savedData.end(),
		crlf.begin(), crlf.end());
		
	if (it == _savedData.end())
		return ;

	std::string	requestLine(_savedData.begin() + _bufferIndex, it);
	std::istringstream iss(requestLine);
	
	if (iss >> _method >> _uri >> _version)
	{
		_bufferIndex += requestLine.size() + 2;
		_currentState = READING_HEADERS;
	}
	else
		_currentState = ERROR;
}
