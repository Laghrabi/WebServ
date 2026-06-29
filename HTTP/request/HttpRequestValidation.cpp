/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestValidation.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 16:15:41 by claghrab          #+#    #+#             */
/*   Updated: 2026/06/29 17:33:03 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

/**
 * @brief Utility to decode percent-encoded characters in a string.
 * * Replaces "%XX" with the corresponding ASCII character.
 * @param target The string to decode in-place.
 * @return true if encoding is valid, false otherwise.
 */
bool HttpRequest::decodeString(std::string& target) {
    size_t i = 0;
    while ((i = target.find('%', i)) != std::string::npos) {
        if (i + 2 >= target.length() || 
            !std::isxdigit(static_cast<unsigned char>(target[i + 1])) || 
            !std::isxdigit(static_cast<unsigned char>(target[i + 2]))) {
                return false;
        }
        std::string hexStr = target.substr(i + 1, 2);
        long convertedHex = std::strtol(hexStr.c_str(), NULL, 16);
        target.replace(i, 3, 1, static_cast<char>(convertedHex));
        i++;
    }
    return true;
}

/**
 * @brief Decodes the route URI and handles errors with status codes.
 * @return true if success, false and sets 400 status on failure.
 */
bool HttpRequest::uriDecode() {
    if (decodeString(_routeUri) == false) {
        _statusCode = BAD_REQUEST;
        _currentState = ERROR;
        return false;
    }
	return (true);
}

/**
 * @brief Validates the HTTP method against supported server methods.
 * Enforces that only GET, POST, and DELETE are processed. Unsupported 
 * methods transition the FSM to ERROR and set the status code to 
 * 501 (Not Implemented).
 * @return true if the method is supported, false otherwise.
 */
bool HttpRequest::validateMethod() {
    if (_method != "GET" && _method != "POST" && _method != "DELETE") {
        _statusCode = NOT_IMPLEMENTED;
        _currentState = ERROR;
        return (false); 
    }
    return (true);
}

/**
 * @brief Validates the HTTP protocol version.
 * Enforces strict HTTP/1.1 compliance. Unsupported versions transition 
 * the FSM to ERROR and set the status code to 505 (HTTP Version Not Supported).
 * @return true if the version is HTTP/1.1, false otherwise.
 */
bool	HttpRequest::validateVersion() {
	if (_version != "HTTP/1.1") {
		_statusCode = HTTP_VERSION_NOT_SUPPORTED;
        _currentState = ERROR;
        return (false); 
	}
	return (true);
}

/**
 * @brief Separates the request URI into the core route path and the query string.
 * * Scans the URI for the '?' delimiter. If found, the portion before the delimiter 
 * is stored in `_routeUri` (used for path matching), and the portion after is 
 * stored in `_queryString` (used for parameters). If no delimiter exists, the 
 * entire URI is treated as the route path and the query string is cleared.
 * * @return true always, as this operation is deterministic and non-failing.
 */
bool HttpRequest::splitQueryString() {
    size_t queryPos = _uri.find('?');
    
    if (queryPos != std::string::npos) {
        _routeUri = _uri.substr(0, queryPos);
        _queryString = _uri.substr(queryPos + 1);
    } else {
        _routeUri = _uri;
        _queryString = "";
    }
    return (true);
}

/**
 * @brief Parses the query string into a map of key-value parameters.
 * * Splits the query string by '&' and '=' delimiters. Supports multi-value 
 * parameters by comma-separating values for duplicate keys. Decodes 
 * percent-encoded characters in both keys and values.
 * @return true on success.
 */
bool	HttpRequest::parseQueryParams() {
	if (_queryString.empty())
		return (true);
		
	size_t	startPos = 0;
	size_t	ampPos = _queryString.find('&');

	while (startPos < _queryString.length()) {
		std::string	pair;
		
		if (ampPos != std::string::npos) {
			pair = _queryString.substr(startPos, ampPos - startPos);
			startPos = ampPos + 1;
			ampPos = _queryString.find('&', startPos);
		} else {
			pair = _queryString.substr(startPos);
			startPos = _queryString.length();
		}
		
		size_t	eqPos = pair.find('=');
		if (eqPos != std::string::npos) {
			std::string	key = pair.substr(0, eqPos);
			std::string	value = pair.substr(eqPos + 1);
			decodeString(key);
    		decodeString(value);
			if (_queryParams.find(key) != _queryParams.end()) {
        		_queryParams[key] += "," + value;
    		} else {
        		_queryParams[key] = value;
   			}
		} else {
			_queryParams[pair] = "";
		}
	}
	return (true);
}

/**
 * @brief Normalizes the URI path to resolve '.' and '..' segments.
 * * Uses a stack-based algorithm to process path segments. Resolves relative 
 * navigation and prevents directory traversal attacks by returning an error 
 * if a '..' attempts to go above the root.
 * @return true if valid, false and sets 403 status if traversal is invalid.
 */
bool    HttpRequest::normalizeUri() {
    std::vector<std::string>    stack;
    size_t                      start = 0;
    size_t                      end = 0;

    while (start < _routeUri.length()) {
        end = _routeUri.find('/', start);
        if (end == std::string::npos)
            end = _routeUri.length();
        std::string segment = _routeUri.substr(start, end - start);
        start = end + 1;
        if (segment == "" || segment == ".")
            continue ;
        else if (segment == "..") {
            if (!stack.empty())
                stack.pop_back();
            else {
                _statusCode = FORBIDDEN;
                _currentState = ERROR;
                return (false);
            }
        } else
            stack.push_back(segment);         
    }
    _routeUri = "";
    for (int i = 0; i < stack.size(); ++i)
        _routeUri += "/" + stack[i];
    if (_routeUri.empty())
        _routeUri = "/";
    return (true);
}
