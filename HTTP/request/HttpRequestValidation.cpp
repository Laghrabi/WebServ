/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestValidation.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/21 16:15:41 by claghrab          #+#    #+#             */
/*   Updated: 2026/06/27 17:47:15 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

/**
 * @brief Decodes percent-encoded characters in the HTTP request URI.
 * * Iterates through the URI string and replaces valid percent-encoded 
 * hexadecimal sequences (e.g., "%20") with their corresponding ASCII 
 * characters. If an invalid or incomplete percent-encoding sequence 
 * is detected, it transitions the finite state machine to the ERROR state.
 * * @return true if the URI was successfully decoded; false if malformed 
 * percent-encoding was encountered.
 */
bool HttpRequest::uriDecode() {
	size_t	i = 0;
	while ((i = _uri.find('%', i)) != std::string::npos) {
		if (i + 2 >= _uri.length()|| 
            !std::isxdigit(static_cast<unsigned char>(_uri[i + 1])) || 
            !std::isxdigit(static_cast<unsigned char>(_uri[i + 2]))) {
				_statusCode = BAD_REQUEST;
				_currentState = ERROR;
				return (false);
		}
		std::string	hexStr = _uri.substr(i + 1, 2);
		long	convertedHex = std::strtol(hexStr.c_str(), NULL, 16);
		char	convertedChar = static_cast<char>(convertedHex);
		_uri.replace(i, 3, 1, convertedChar);
		i++;
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
