#include "HttpRequest.hpp"
#include "RouteManager.hpp"

HttpRequest::HttpRequest() : _statusCode(OK), _currentState(READING_REQUEST_LINE), _bufferIndex(0),
	_contentLength(0),  _chunkedSize(0), _server(NULL), _bodyBytesWritten(0){}

	/**
	 * @brief Default constructor.
	 * 
	 * Initializes a new HTTP request, setting the initial parsing state 
	 * to READING_REQUEST_LINE and the buffer index to 0.
	 */
	HttpRequest::HttpRequest(const Config::ServerRange& serverRange, const Server::IPort& clientEndPoint) : _statusCode(OK), _currentState(READING_REQUEST_LINE), _bufferIndex(0),
	_contentLength(0),  _chunkedSize(0), _server(NULL), _serverRange(serverRange), _clientEndPoint(clientEndPoint), _bodyBytesWritten(0) {}

	/**
	 * @brief Copy constructor for HttpRequest.
	 * Performs a member-wise copy of the request state, including buffer data,
	 * headers, query parameters, and FSM status.
	 * @param other The source HttpRequest object to copy.
	 */
	HttpRequest::HttpRequest(const HttpRequest& other) : 
		_statusCode(other._statusCode),
		_currentState(other._currentState),
		_savedData(other._savedData),
		_bufferIndex(other._bufferIndex),
		_method(other._method),
		_uri(other._uri),
		_routeUri(other._routeUri),
		_EncodedRouteUri(other._EncodedRouteUri),
      	_EncodedUriSegments(other._EncodedUriSegments),
      	_UriSegments(other._UriSegments),
		_queryString(other._queryString),
		_host(other._host),
		_queryParams(other._queryParams),
		_version(other._version),
		_headers(other._headers),
		_contentLength(other._contentLength),
		_chunkedSize(other._chunkedSize),
		_body(other._body),
		// _client_max_body_size(other._client_max_body_size),
		_server(other._server),
		_serverRange(other._serverRange),
		_bodyFilePath(other._bodyFilePath),
		_clientEndPoint(other._clientEndPoint),
		_bodyBytesWritten(other._bodyBytesWritten),
		_routeResult(other._routeResult){}

		/**
		 * @brief Copy assignment operator for HttpRequest.
		 * Safely updates the current object's state to match the source object.
		 * Prevents self-assignment and performs a deep copy of all internal members.
		 * @param other The source HttpRequest object to assign from.
		 * @return A reference to the current object.
		 */
		HttpRequest& HttpRequest::operator=(const HttpRequest& other) {
			if (this != &other) {
				_statusCode = other._statusCode;
				_currentState = other._currentState;
				_savedData = other._savedData;
				_bufferIndex = other._bufferIndex;
				_method = other._method;
				_uri = other._uri;
				_routeUri = other._routeUri;
				_EncodedRouteUri = other._EncodedRouteUri;
        		_EncodedUriSegments = other._EncodedUriSegments;
        		_UriSegments = other._UriSegments;
				_queryString = other._queryString;
				_host = other._host;
				_queryParams = other._queryParams;
				_version = other._version;
				_headers = other._headers;
				_contentLength = other._contentLength;
				_chunkedSize = other._chunkedSize;
				_body = other._body;
				_server = other._server;
				_serverRange = other._serverRange;
				_clientEndPoint = other._clientEndPoint;
				_bodyFilePath = other._bodyFilePath;
				_bodyBytesWritten = other._bodyBytesWritten;
				_routeResult = other._routeResult;
			}
			return (*this);
		}



/**
 * @brief Destructor.
 */
HttpRequest::~HttpRequest() {}

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
				if (parseRequestLine() == false ||
						validateMethod() == false ||
						validateVersion() == false ||
						splitQueryString() == false ||
						parseQueryParams() == false ||
						normalizeUri() == false)
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
			case READING_CHUNK_SIZE:
				if (parseChunkSize() == false)
					return ;
				break ;
			case READING_CHUNK_DATA:
				if (parseChunkData() == false)
					return ;
				break ;
			case READING_TRAILERS:
				if (parseHeaders() == false)
					return ;
				break ;
			case FINISHED:
			case ERROR:
				return ;
		}
	}
}

/**
 * @brief Extracts the HTTP method, URI, and version from the first line.	_headers.erase("host");
 * 
 * @return true if the request line is fully parsed; false if incomplete or malformed.
 */
bool	HttpRequest::parseRequestLine()
{
	// std::cout << "parseRequestLine()\n";
	const std::string	crlf = "\r\n";
	std::string			trailingGarbage;

	std::vector<char>::iterator it = std::search(
			_savedData.begin() + _bufferIndex, _savedData.end(),
			crlf.begin(), crlf.end());

	if (it == _savedData.end())
		return (false);

	std::string	requestLine(_savedData.begin() + _bufferIndex, it);

	size_t spaceOne = requestLine.find(' ');
    if (spaceOne == std::string::npos)
    {;
        _statusCode = BAD_REQUEST;
        _currentState = ERROR;
        return (false);
    }

    size_t spaceTwo = requestLine.find(' ', spaceOne + 1);
    if (spaceTwo == std::string::npos)
    {
        _statusCode = BAD_REQUEST;
        _currentState = ERROR;
        return (false);
    }

    if (spaceTwo == spaceOne + 1 || requestLine.find(' ', spaceTwo + 1) != std::string::npos)
    {
        _statusCode = BAD_REQUEST;
        _currentState = ERROR;
        return (false);
    }

    _method = requestLine.substr(0, spaceOne);
    _uri = requestLine.substr(spaceOne + 1, spaceTwo - spaceOne - 1);
    _version = requestLine.substr(spaceTwo + 1);

    if (_uri.empty() || _uri.at(0) != '/')
    {
        _statusCode = BAD_REQUEST;
        _currentState = ERROR;
        return (false);
    }

    _bufferIndex += requestLine.size() + 2;
    _currentState = READING_HEADERS;
    _savedData.erase(_savedData.begin(),
            _savedData.begin() + _bufferIndex);
    _bufferIndex = 0;
    return (true);
}

/**
 * @brief Parses incoming HTTP headers line by line until the empty line boundary is reached.
 * * Extracts individual header lines by searching for the CRLF boundary. It validates the 
 * header key syntax (strictly forbidding whitespace before the colon per RFC 7230) and 
 * normalizes the key to lowercase for case-insensitive lookups. The value is trimmed of 
 * optional whitespace (OWS). If an empty line is encountered (indicating the end of the 
 * headers), it triggers the header validation routine.
 * * @return true if a header was successfully parsed or if header parsing is complete; 
 * false if more data is needed from the socket or if a syntax error forces the ERROR state.
 */
bool	HttpRequest::parseHeaders()
{
	// std::cout << "parseHeaders()\n";
	const std::string	crlf = "\r\n";
	std::vector<char>::iterator it = std::search(
			_savedData.begin() + _bufferIndex, _savedData.end(),
			crlf.begin(), crlf.end());	
	if (it == _savedData.end())
		return (false);

	std::string headerLine(_savedData.begin() + _bufferIndex, it);
	if (headerLine.empty()) {
		if (_currentState == READING_TRAILERS) {
			_bufferIndex += 2;
			_savedData.erase(_savedData.begin(),
					_savedData.begin() + _bufferIndex);
			_bufferIndex = 0;
			_currentState = FINISHED;
			return (true);
		}	
		return (validateHeaders());
	}

	size_t	colonPos = headerLine.find(':');
	if (colonPos == std::string::npos) {
		_statusCode = BAD_REQUEST;
		_currentState = ERROR;
		return (false);
	} else {
		std::string	key = headerLine.substr(0, colonPos);
		if (key.empty() || key.find_first_of(" \t") != std::string::npos) {
			_statusCode = BAD_REQUEST;
			_currentState = ERROR;
			return (false);
		}

		std::string	value = headerLine.substr(colonPos + 1);
		value = trimSpaces(value);
		if (value.empty()) {
            _statusCode = BAD_REQUEST;
            _currentState = ERROR;
            return (false);
        }
		std::transform(key.begin(), key.end(), key.begin(), safeToLower);

		if (_currentState == READING_TRAILERS) {
            if (key == "host" || key == "content-length" || key == "transfer-encoding") {
                _statusCode = BAD_REQUEST;
                _currentState = ERROR;
                return (false);
            }
        }
		if (_headers.find(key) != _headers.end()) {
			if (key == "host" || key == "content-length" ||
					key == "content-type" || key == "transfer-encoding") {
				_statusCode = BAD_REQUEST;
				_currentState = ERROR;
				return (false);
			}
			_headers[key] += ", " + value;
		} else {
			_headers[key] = value;
		}
		_bufferIndex += headerLine.size() + 2;
		return(true);
	}
}

/**
 * @brief Validates the parsed headers to determine the request payload format and size.
 * * Enforces strict RFC 7230 routing: rejects requests that contain both `Content-Length` 
 * and `Transfer-Encoding`. 
 * - For `Content-Length`: validates the format (digits only), protects against integer 
 * overflow during conversion, and ensures the payload does not exceed `_MAX_BODY_SIZE`.
 * - For `Transfer-Encoding`: ensures the encoding type is exclusively "chunked".
 * * Upon successful validation, it skips the final CRLF and transitions the FSM to either 
 * READING_BODY or READING_CHUNK_SIZE.
 * * @return true if headers are valid and the FSM state was successfully advanced; 
 * false if validation fails (e.g., conflicting headers, non-numeric size, oversized body), 
 * which immediately transitions the FSM to the ERROR state.
 */
bool	HttpRequest::validateHeaders() {
	// std::cout << "validateHeaders()\n";
	std::map<std::string, std::string>::iterator itHost = _headers.find("host");
	std::map<std::string, std::string>::iterator itContentLength = _headers.find("content-length");
	std::map<std::string, std::string>::iterator itTransferEncoding = _headers.find("transfer-encoding");
	if (itHost == _headers.end()) {
		_statusCode = BAD_REQUEST;
		_currentState = ERROR;
		return false;
	}
	_server = findServer(itHost->second);
	_headers.erase("host");
	if (_method == "POST" && itContentLength == _headers.end() && itTransferEncoding == _headers.end()) {
		_statusCode = BODY_LENGTH_REQUIRED;
		_currentState = ERROR;
		return false;
	}
	if (itContentLength != _headers.end() && itTransferEncoding != _headers.end()) {
		_statusCode = BAD_REQUEST;
		_currentState = ERROR;
		return (false);
	}
	else if (itContentLength != _headers.end()) {
		std::string	clValue = itContentLength->second;
		if (clValue.empty() || clValue.find_first_not_of("0123456789") != std::string::npos) {
			_statusCode = BAD_REQUEST;
			_currentState = ERROR;
			return (false);
		}
		std::istringstream iss(clValue);
		if (!(iss >> _contentLength)) {
			_statusCode = BAD_REQUEST;
			_currentState = ERROR;
			return (false);
		}
		_currentState = READING_BODY;
	} 
	else if (itTransferEncoding != _headers.end()) {
		std::string	teValue = itTransferEncoding->second;
		if (teValue != "chunked") {
			_statusCode = NOT_IMPLEMENTED;
			_currentState = ERROR;
			return (false);
		}
		_currentState = READING_CHUNK_SIZE;
	}
	_bufferIndex += 2;
	if (_currentState == READING_HEADERS) {
		_currentState = FINISHED;
		_savedData.erase(_savedData.begin(),
				_savedData.begin() + _bufferIndex);
		_bufferIndex = 0;
	}
	_savedData.erase(_savedData.begin(),
            _savedData.begin() + _bufferIndex);
    _bufferIndex = 0;
	RouteManager route_manager;
	route_manager.processRequest(*this);
	return (true);
}

/**
 * @brief Processes a fixed-length HTTP request body.
 * * Reads data from the internal buffer and streams it to the designated file.
 * Handles the logic of consuming specific byte ranges, managing memory cleanup 
 * after consumption, and transitioning the request state to FINISHED.
 * @return true if the body is fully parsed, false if more data is required or an error occurs.
 */
bool	HttpRequest::parseBody()
{
	// std::cout << "parseBody()\n";
	if (_contentLength == 0) {
		_savedData.erase(_savedData.begin(), _savedData.begin() + _bufferIndex);
		_bufferIndex = 0;
		_currentState = FINISHED;
		return (true);
	} else {
		if (!openBodyStream())
        	return (false); 

		size_t	avaiBytes = _savedData.size() - _bufferIndex;
		size_t	bytesNeeded = _contentLength - _bodyBytesWritten;
		size_t bytesToWrite = std::min(avaiBytes, bytesNeeded);

		_bodyStream.write(_savedData.data() + _bufferIndex, bytesToWrite);
		if (_bodyStream.fail()) {
			_bodyStream.close();
			std::remove(_bodyFilePath.c_str());
            _statusCode = INTERNAL_SERVER_ERROR;
            _currentState = ERROR;
            return (false);
        }

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

/**
 * @brief Parses the size header for a chunked transfer-encoding request.
 * * Searches for the CRLF-delimited chunk size line, validates that the size
 * is a valid hexadecimal value, and enforces payload size limits. 
 * Transitions the parser state to READING_CHUNK_DATA or READING_TRAILERS.
 * @return true if the size was successfully parsed, false otherwise (or if data is incomplete).
 */
bool HttpRequest::parseChunkSize() {
	const std::string	crlf = "\r\n";

	std::vector<char>::iterator it = std::search(
			_savedData.begin() + _bufferIndex, _savedData.end(),
			crlf.begin(), crlf.end());

	if (it == _savedData.end())
		return (false);

	std::string	 chunkedLine(_savedData.begin() + _bufferIndex, it);
	const size_t semiPos = chunkedLine.find(';');
	std::string sizePart;
	if (semiPos != std::string::npos)
		sizePart = chunkedLine.substr(0, semiPos);
	else
		sizePart = chunkedLine;
	if (sizePart.empty() || sizePart.find_first_not_of("0123456789ABCDEFabcdef") != std::string::npos) {
		if (_bodyStream.is_open()) {
            _bodyStream.close();
            std::remove(_bodyFilePath.c_str());
		}
		_statusCode = BAD_REQUEST;
		_currentState = ERROR;
		return (false);
	}
	std::istringstream iss(sizePart);
	if (!(iss >> std::hex >> _chunkedSize)) {
		if (_bodyStream.is_open()) {
            _bodyStream.close();
            std::remove(_bodyFilePath.c_str());
		}
		_statusCode = BAD_REQUEST;
		_currentState = ERROR;
		return (false);
	} else if (_chunkedSize != 0) {
		_currentState = READING_CHUNK_DATA;
		_bufferIndex += chunkedLine.size() + 2;
		return (true);
	} else {
		if (_bodyStream.is_open()) {
            _bodyStream.close();
        }
		_currentState = READING_TRAILERS;
		_bufferIndex += chunkedLine.size() + 2;
		return (true);
	}
}

/**
 * @brief Parses and writes the actual data payload for a chunked request.
 * * Verifies that the full chunk (including trailing CRLF) is available in the buffer.
 * Performs error checking on chunk integrity and payload size limits before
 * streaming data to the destination file.
 * @return true if the chunk was processed successfully, false if incomplete or in error.
 */
bool	HttpRequest::parseChunkData() {
	if ((_savedData.size() - _bufferIndex) < (_chunkedSize + 2))
		return (false);
	if (_savedData[_bufferIndex + _chunkedSize] != '\r' ||
			_savedData[_bufferIndex + _chunkedSize + 1] != '\n') {
		if (_bodyStream.is_open()) {
            _bodyStream.close();
            std::remove(_bodyFilePath.c_str());
        }
		_statusCode = BAD_REQUEST;
		_currentState = ERROR;
		return (false);
	}

	if (!openBodyStream())
        return false;
		
	_bodyStream.write(_savedData.data() + _bufferIndex, _chunkedSize);
	if (_bodyStream.fail()) {
        _bodyStream.close();
        std::remove(_bodyFilePath.c_str());
        _statusCode = INTERNAL_SERVER_ERROR;
        _currentState = ERROR;
        return (false);
    }

	_bodyBytesWritten += _chunkedSize;
	size_t	totalConsumedBytes = _bufferIndex + _chunkedSize + 2;
	_savedData.erase(_savedData.begin(),
			_savedData.begin() + totalConsumedBytes);
	_bufferIndex = 0;
	_currentState = READING_CHUNK_SIZE;
	return (true);
}
