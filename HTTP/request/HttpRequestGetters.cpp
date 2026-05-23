/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestGetters.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/22 17:57:16 by claghrab          #+#    #+#             */
/*   Updated: 2026/05/22 18:04:37 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

const std::string&	HttpRequest::getMethod() const {
	return (_method);
}

const std::string& HttpRequest::getUri() const {
	return (_uri);
}

const std::string& HttpRequest::getVersion() const {
	return (_version);
}

const std::map<std::string, std::string>& HttpRequest::getHeaders() const {
	return (_headers);
}

std::string HttpRequest::getHeader(const std::string& key) const {
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);
	if (it != _headers.end())
		return (it->second);
	return ("");
}

const std::string& HttpRequest::getBodyFilePath() const {
	return (_bodyFilePath);
}

ParseState HttpRequest::getCurrentState() const {
	return (_currentState);
}

std::vector<char>	HttpRequest::getLeftoverData() const {
	return (_savedData);
}
