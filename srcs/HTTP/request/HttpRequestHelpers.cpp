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
