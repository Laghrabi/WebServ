#include "../../../include/webserver.hpp"

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
	
	std::string host = name;
 	if (!host.empty() && host[0] == '[') {
 		const size_t rb = host.find(']');
 		if (rb != std::string::npos)
 			host = host.substr(1, rb - 1);
 	} else {
 		const size_t colon = host.find(':');
 		if (colon != std::string::npos)
 			host = host.substr(0, colon);
 	}

	for (; begin != end; ++begin) {
		const Server *server = &begin->second;
		if (server->hasServerName(host)) {
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
void HttpRequest::tokenizeUri() { 
    size_t start = 0;
    size_t end = 0;
    
    while ((end = _routeUri.find('/', start)) != std::string::npos) {
        std::string segment = _routeUri.substr(start, end - start);
        if (!segment.empty()) {
            _uriSegments.push_back(segment);
        }
        start = end + 1;
    }
    
    std::string lastSegment = _uri.substr(start);
    if (!lastSegment.empty()) {
        _uriSegments.push_back(lastSegment);
    }
}
