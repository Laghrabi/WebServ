/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:30:50 by claghrab          #+#    #+#             */
/*   Updated: 2026/05/12 14:58:04 by claghrab         ###   ########.fr       */
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

