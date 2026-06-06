/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: claghrab <claghrab@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/12 14:30:50 by claghrab          #+#    #+#             */
/*   Updated: 2026/06/04 14:53:48 by claghrab         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"
#include "../../Utils/StringUtils.hpp"


int HttpRequest::_fileCounter = 0;

/**
  * @brief Default constructor.
  * 
  * Initializes a new HTTP request, setting the initial parsing state 
  * to READING_REQUEST_LINE and the buffer index to 0.
  */
HttpRequest::HttpRequest() : _currentState(READING_REQUEST_LINE), _bufferIndex(0),
							_contentLength(0), _bodyBytesWritten(0) {}

/**
  * @brief Destructor.
  */
HttpRequest::~HttpRequest() {
	if (_bodyStream.is_open())
		_bodyStream.close();
	if (_currentState != FINISHED)
		cleanupTempFile();
}

/**
 * @brief Appends incoming data to the internal buffer and drives the parsing state machine.
 * 
 * @param rawBuffer The raw byte chunk received from the socket.
 */
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
				if (parseRequestLine() == false)
					return ;
				break ;
			case READING_HEADERS:
				if (parseHeaders() == false)
					return ;
				break ;
			case READING_BODY:
				if (parseBody() == false)
					return ;
				break ;
			case FINISHED:
			case ERROR:
				return ;
		}
	}
}

/**
 * @brief Extracts the HTTP method, URI, and version from the first line.
 * 
 * @return true if the request line is fully parsed; false if incomplete or malformed.
 */
bool	HttpRequest::parseRequestLine()
{
	const std::string	crlf = "\r\n";
	std::string			trailingGarbage;

	std::vector<char>::iterator it = std::search(
		_savedData.begin() + _bufferIndex, _savedData.end(),
		crlf.begin(), crlf.end());
		
	if (it == _savedData.end())
		return (false);

	std::string	requestLine(_savedData.begin() + _bufferIndex, it);
	std::istringstream iss(requestLine);
	
	if (iss >> _method >> _uri >> _version)
	{
		if (iss >> trailingGarbage) {
    		_currentState = ERROR;
    		return (false);
		}
		_bufferIndex += requestLine.size() + 2;
		_currentState = READING_HEADERS;
		return (true);
	}
	else
	{
		_currentState = ERROR;
		return (false);
	}
}

/**
 * @brief Parses a single header line, stores key-value pairs, and detects the header-body boundary.
 * 
 * @return true if a header was parsed or headers are complete; false if incomplete or malformed.
 */
bool	HttpRequest::parseHeaders()
{
	const std::string	crlf = "\r\n";

	std::vector<char>::iterator it = std::search(
		_savedData.begin() + _bufferIndex, _savedData.end(),
		crlf.begin(), crlf.end());
		
	if (it == _savedData.end())
		return (false);
	
	std::string headerLine(_savedData.begin() + _bufferIndex, it);
	if (headerLine.empty())
	{
		std::map<std::string, std::string>::iterator it = _headers.find("content-length");
    	if (it != _headers.end()) {
        	std::istringstream iss(it->second);
        	iss >> _contentLength;
    	}
		_currentState = READING_BODY;
		_bufferIndex += 2;
		return (true);
	}
	
	size_t	colonPos = headerLine.find(':');
	if (colonPos == std::string::npos)
	{
		_currentState = ERROR;
		return (false);
	}
	else
	{
		std::string	key = headerLine.substr(0, colonPos);
		if (key.empty() || key.find_first_of(" \t") != std::string::npos) {
			_currentState = ERROR;
			return (false);
		}
		std::string	value = headerLine.substr(colonPos + 1);
		std::transform(key.begin(), key.end(), key.begin(), ::tolower);
		_headers[key] = trimLeadingSpaces(value);
		_bufferIndex += headerLine.size() + 2;

		return(true);
	}
}

/**
 * @brief Streams body data into a temporary file to handle large payloads efficiently.
 * 
 * @return true if the entire body has been saved; false if waiting for more data or on stream error.
 */
bool	HttpRequest::parseBody()
{
	if (_contentLength == 0) {
		_currentState = FINISHED;
		return (true);
	} else {
		if (!_bodyStream.is_open()) {
			_fileCounter++;
			std::time_t timestamp = std::time(NULL);
			std::ostringstream oss;
            oss << "/tmp/upload_" << timestamp << "_" << _fileCounter << ".bin";
            _bodyFilePath = oss.str();
			_bodyStream.open(_bodyFilePath.c_str(), std::ios::binary);
			
			if (!_bodyStream.is_open()) {
				_currentState = ERROR;
				return (false);
			}
		}
		size_t	avaiBytes = _savedData.size() - _bufferIndex;
		size_t	bytesNeeded = _contentLength - _bodyBytesWritten;
		size_t bytesToWrite = std::min(avaiBytes, bytesNeeded);
		_bodyStream.write(_savedData.data() + _bufferIndex, bytesToWrite);
		_bodyBytesWritten += bytesToWrite;
		size_t	totalConsumedBytes = _bufferIndex + bytesToWrite;
		_savedData.erase(_savedData.begin(), _savedData.begin() + totalConsumedBytes);
		_bufferIndex = 0;
		if (_bodyBytesWritten == _contentLength) {
			_bodyStream.close();
			_currentState = FINISHED;
			return (true);
		} else
			return (false);
	}
}

void	HttpRequest::cleanupTempFile() {
	if (!_bodyFilePath.empty())
		std::remove(_bodyFilePath.c_str());
}
