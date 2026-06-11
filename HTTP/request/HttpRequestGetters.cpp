/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestGetters.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 17:57:16 by claghrab          #+#    #+#             */
/*   Updated: 2026/06/11 17:31:42 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

/**
 * @brief Retrieves the HTTP method of the parsed request.
 * * @return A constant reference to the method string (e.g., "GET", "POST", "DELETE").
 */
const std::string&	HttpRequest::getMethod() const {
	return (_method);
}

/**
 * @brief Retrieves the requested URI.
 * * @return A constant reference to the URI string (e.g., "/index.html").
 */
const std::string& HttpRequest::getUri() const {
	return (_uri);
}

/**
 * @brief Retrieves the HTTP protocol version.
 * * @return A constant reference to the version string (e.g., "HTTP/1.1").
 */
const std::string& HttpRequest::getVersion() const {
	return (_version);
}

/**
 * @brief Retrieves the parsed body (payload) of the HTTP request.
 * * @return A constant reference to the byte vector containing the request body.
 */
const std::vector<char>& HttpRequest::getBody() const {
    return _body;
}

/**
 * @brief Retrieves all parsed HTTP headers.
 * * @return A constant reference to the map containing all normalized 
 * key-value header pairs.
 */
const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
	return (_headers);
}

/**
 * @brief Retrieves the value of a specific HTTP header.
 * * Performs a safe, case-insensitive lookup by normalizing the provided key 
 * to lowercase before searching the internal header map.
 * * @param key The name of the header to retrieve (e.g., "Content-Length").
 * @return The value of the header if found; otherwise, an empty string.
 */
std::string HttpRequest::getHeader(const std::string& key) const {
	std::string normalized = key;
 	std::transform(normalized.begin(), normalized.end(), normalized.begin(), safeToLower);
	std::map<std::string, std::string>::const_iterator it = _headers.find(normalized);
	if (it != _headers.end())
		return (it->second);
	return ("");
}

/**
 * @brief Retrieves the current status of the parsing finite state machine.
 * * @return The current ParseState (e.g., FINISHED, ERROR, READING_BODY).
 */
ParseState HttpRequest::getCurrentState() const {
	return (_currentState);
}

/**
 * @brief Retrieves any unparsed data remaining in the read buffer.
 * * This is critical for HTTP pipelining, as it extracts bytes belonging to 
 * subsequent requests that were read from the socket but not consumed by 
 * the current request's parsing lifecycle.
 * * @return A vector of characters containing the leftover pipeline data.
 */
std::vector<char>	HttpRequest::getLeftoverData() const {
	return (std::vector<char>(_savedData.begin() + _bufferIndex,
			_savedData.end()));
}
