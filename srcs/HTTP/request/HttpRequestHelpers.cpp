#include "HttpRequest.hpp"

/**
 * @brief Safely converts a given character to lowercase.
 * * This utility function wraps `std::tolower` to ensure safe type conversion. 
 * By explicitly casting the input to `unsigned char` before passing it to `std::tolower`, 
 * it prevents the undefined behavior that occurs when passing negative signed `char` 
 * values (such as extended ASCII characters) to standard Cctype library functions.
 * * @param c The character to convert.
 * @return The lowercase equivalent of the character if one exists; otherwise, 
 * the character is returned unchanged.
 */
char	safeToLower(char c) {
	return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

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
 * @brief Searches for a server configuration by its Host header name.
 * * Iterates through the pre-filtered range of servers associated with the 
 * request's listening address. Returns the first matching server based on 
 * the 'server_name' directive, or defaults to the first server in the range.
 * @param name The server name from the HTTP 'Host' header.
 * @return A pointer to the matching Server, or the default Server if no match.
 */
const Server* HttpRequest::findServer(const std::string& name) {
	
	Config::ServerMultiMapConstIter begin = _serverRange.first;
	const Config::ServerMultiMapConstIter& end = _serverRange.second;
	
	_host = name;
 	if (!_host.empty() && _host[0] == '[') {
 		const size_t rb = _host.find(']');
 		if (rb != std::string::npos)
 			_host = _host.substr(1, rb - 1);
 	} else {
 		const size_t colon = _host.find(':');
 		if (colon != std::string::npos)
 			_host = _host.substr(0, colon);
 	}

	for (; begin != end; ++begin) {
		const Server *server = &begin->second;
		if (server->hasServerName(_host)) {
			return (server);
		}
	}
	return (&_serverRange.first->second);
}

/**
 * @brief Splits the request URI into individual segments based on the '/' delimiter.
 * * Populates the `_uriSegments` member variable, filtering out empty segments 
 * caused by multiple consecutive slashes.
 */
void HttpRequest::tokenizeUri(std::vector<std::string>& segments) const {
    size_t start = 0;
    size_t end = 0;
    
    while ((end = _routeUri.find('/', start)) != std::string::npos) {
        std::string segment = _routeUri.substr(start, end - start);
        if (!segment.empty()) {
            segments.push_back(segment);
        }
        start = end + 1;
    }
    
    std::string lastSegment = _routeUri.substr(start);
    if (!lastSegment.empty()) {
        segments.push_back(lastSegment);
    }
}

/**
 * @brief Resets the HTTP request state for subsequent requests on the same connection.
 * 
 * Clears all parsed headers, body data, and URI segments, and resets the 
 * finite state machine back to READING_REQUEST_LINE.
 * 
 * Note: _savedData is intentionally NOT cleared to preserve pipelined requests 
 * or partial data read for the next request. _serverRange is also preserved 
 * as it represents the static server configuration for the connection.
 */
void HttpRequest::reset() {
    _statusCode = OK;
    _currentState = READING_REQUEST_LINE;
    _bufferIndex = 0; 
    _method.clear();
    _uri.clear();
    _routeUri.clear();
    _EncodedRouteUri.clear();
    _queryString.clear();
    _version.clear();
    _EncodedUriSegments.clear();
    _UriSegments.clear();
    _queryParams.clear();
    _headers.clear();
    _body.clear();
    _contentLength = 0;
    _chunkedSize = 0;
    _bodyBytesWritten = 0;
    // _client_max_body_size = _DEFAULT_BODY_SIZE;
    _server = NULL;
    _routeResult.action = NONE;
    _routeResult.statusCode = OK;
    _routeResult.targetPath.clear();
}

void HttpRequest::printHttpStatus(HttpStatus status) {
    std::cout << "**********HTTP PARSING RESULT**********" << std::endl;
    switch (status) {
        case OK:
            std::cout << "Status [200]: OK\n";
            break;
        case MOVED_PERMANENTLY:
            std::cout << "Status [301]: Moved Permanently\n";
            break;
        case FOUND:
            std::cout << "Status [302]: Found\n";
            break;
        case TEMPORARY_REDIRECT:
            std::cout << "Status [307]: Temporary Redirect\n";
            break;
        case BAD_REQUEST:
            std::cerr << "Error [400]: Bad Request\n";
            break;
        case FORBIDDEN:
            std::cerr << "Error [403]: Forbidden\n";
            break;
        case NOT_FOUND:
            std::cerr << "Error [404]: Not Found\n";
            break;
        case METHOD_NOT_ALLOWED:
            std::cerr << "Error [405]: Method Not Allowed\n";
            break;
        case BODY_LENGTH_REQUIRED:
            std::cerr << "Error [411]: Length Required\n";
            break;
        case PAYLOAD_TOO_LARGE:
            std::cerr << "Error [413]: Payload Too Large\n";
            break;
        case URI_TOO_LONG:
            std::cerr << "Error [414]: URI Too Long\n";
            break;
        case INTERNAL_SERVER_ERROR:
            std::cerr << "Error [500]: Internal Server Error\n";
            break;
        case NOT_IMPLEMENTED:
            std::cerr << "Error [501]: Not Implemented\n";
            break;
        case HTTP_VERSION_NOT_SUPPORTED:
            std::cerr << "Error [505]: HTTP Version Not Supported\n";
            break;
            
        default:
            std::cerr << "Error: Unknown HTTP Status Code (" << status << ")\n";
            break;
    }
}

/**
 * @brief Generates a unique secure file path using 16 bytes from /dev/urandom.
 * 
 * @param directory The target directory (e.g., "/tmp/").
 * @param extension The file extension (e.g., ".bin").
 * @return A securely randomized file path string, or an empty string if it fails.
 */
std::string HttpRequest::generateSecureFileName(const std::string& directory, const std::string& extension) {
    std::ifstream urandom("/dev/urandom", std::ios_base::in | std::ios_base::binary);
    if (!urandom.is_open())
        return ("");

    char buffer[16];
    urandom.read(buffer, 16);
    urandom.close();

    std::ostringstream oss;
    oss << directory;

    for (int i = 0; i < 16; ++i) {
        oss << std::hex << std::setfill('0') << std::setw(2)
            << static_cast<int>(static_cast<unsigned char>(buffer[i]));
    }

    oss << extension;
    return (oss.str());
}

/**
 * @brief Lazy-initializes the file stream for buffering the request body.
 * * Generates a unique, secure temporary file path and opens the stream in binary mode.
 * If initialization fails (due to filename generation errors or filesystem issues), 
 * it updates the request state to ERROR and sets the internal server error status code.
 * @return true if the stream is open or successfully opened, false otherwise.
 */
bool HttpRequest::openBodyStream() {
    if (!_bodyStream.is_open()) {
        _bodyFilePath = generateSecureFileName("/tmp/", ".bin");
        
        if (_bodyFilePath.empty()) {
            _statusCode = INTERNAL_SERVER_ERROR;
            _currentState = ERROR;
            return false;
        }

        _bodyStream.open(_bodyFilePath.c_str(), std::ios::binary);
        
        if (!_bodyStream.is_open()) {
            _statusCode = INTERNAL_SERVER_ERROR;
            _currentState = ERROR;
            return false;
        }
    }
    
    return (true);
}

void HttpRequest::printBodyContent() const {
    if (_bodyFilePath.empty()) {
        std::cout << "[Debug] No body file generated for this request." << std::endl;
        return;
    }

    std::ifstream inFile(_bodyFilePath.c_str(), std::ios::binary);
    
    if (!inFile.is_open()) {
        std::cerr << "[Error] Could not open " << _bodyFilePath << " to print." << std::endl;
        return;
    }

    std::cout << "========== BODY CONTENT START (" << _bodyFilePath << ") ==========\n";

    char buffer[8192]; 
    while (inFile.read(buffer, sizeof(buffer))) {
        // std::cout.write is used instead of '<<' because the body might contain 
        // binary data (like an image) with null terminators that would truncate '<<'.
        std::cout.write(buffer, inFile.gcount());
    }

    // Print the final remaining bytes (if any)
    if (inFile.gcount() > 0) {
        std::cout.write(buffer, inFile.gcount());
    }

    std::cout << "\n=========== BODY CONTENT END ===========\n";
    
    inFile.close();
}

void HttpRequest::debugPrintHeaders(const std::map<std::string, std::string>& headers) const {
    
    std::cout << "---HTTP HEADERS---" << std::endl;
    
    std::map<std::string, std::string>::const_iterator it;
    
    for (it = headers.begin(); it != headers.end(); ++it) {
        std::cout << it->first << ": " << it->second << std::endl;
    }
    
    std::cout << "--------------------" << std::endl;
}
