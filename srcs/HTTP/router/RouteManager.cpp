#include "webserver.hpp"
#include "RouteManager.hpp"
#include "HttpRequest.hpp"

RouteManager::RouteManager() : _locator() {}

RouteManager::RouteManager(const RouteManager& other) : _locator(other._locator) {}

RouteManager& RouteManager::operator=(const RouteManager& other) {
	if (this != &other) {
		_locator = other._locator;
	}
	return *this;
}

RouteManager::~RouteManager() {}

bool RouteManager::isCgi(const std::vector<std::string>& script_path, const RouteConfig* route, const std::string& location) const{
	typedef std::vector<std::string> UriCont;
	// typedef UriCont::iterator UriContIter;
	typedef UriCont::const_iterator UriContConstIter;
	std::string path_info;

	std::string test_path = location;
	FileStatus status;

	// RouteResult result;

	std::string extention;
	UriContConstIter it = script_path.begin();
	for (; it != script_path.end(); ++it) {
		test_path += "/" + *it;
		status.set(test_path);
		if (status.exist()) {
			if (status.isDir()) {
// result. = RESOURCE_DIRECTORY;
#ifdef CGI_DEBUG
				std::cout << "[(CGI) " << test_path << "is a directory\n";
#endif
			}
			else if (route->isCgiScript(*it)) {
				std::cout << "[CGI i found it ext = " << test_path << "]" << "\n";
				std::cout << "[CGI path info = ";

				++it;
	for (; it != script_path.end(); ++it) {
		std::cout << "/" << *it;
	}
	std::cout << "]";
					return (true);
			}
			else {
				// here i have a file (not a directory)
			}
		}
		else {
#ifdef CGI_DEBUG
			std::cout << "[(CGI) path: " << test_path << "is not found ]";
#endif
		}
	}
	std::cout << "[CGI not cgi]";
return (false);
}

/**
 * @brief Processes an HTTP request to determine the appropriate routing action.
 * * Orchestrates the full request lifecycle: matches the URI to a configuration,
 * validates methods, handles redirects, resolves the physical file path, and 
 * identifies the resource type (CGI, File, Directory/Autoindex).
 * @param request The validated HttpRequest object.
 * @return A RouteResult containing the determined action, target path, and status code.
 */
RouteResult RouteManager::processRequest(const HttpRequest& request) const {
	RouteResult result;
	result.statusCode = OK;
	std::string location;

	const RouteConfig* route = matchRoute(request.getUriSegments(), request.getServer(), location);
#ifdef DBUG
	std::cout << "[(ROUTEMANAGER) location is matched: " << location << "]\n";
#endif

	std::vector<std::string> vec;
	std::string str = request.getRouteUri().substr(location.length());
#ifdef DEBUG
	std::cout << "[(CGI): file is " << str << "]\n";
#endif
	HttpRequest::normalizeUriHelper(str, vec);

	isCgi(vec, route, location);
	if (route && !route->isAllowed(request.getMethod())) {
		result.action = ACTION_ERROR;
		result.statusCode = METHOD_NOT_ALLOWED;
		return result;
	}

	if (route && route->doesRedirect()) {
		result.action = ACTION_REDIRECT;
		result.targetPath = route->getRedirection().second;
		result.statusCode = route->getRedirection().first;
		return result;
	}

	std::string physicalPath = _locator.resolvePath(request.getRouteUri(), route, request.getServer());
	ResourceType type = _locator.getResourceType(physicalPath);

	// if (type == RESOURCE_FILE && _locator.isCgiExtension(physicalPath)) {
	//     if (route && route->isCgiEnabled()) {
	//         result.action = ACTION_EXECUTE_CGI;
	//         result.targetPath = physicalPath;
	//         return result;
	//     }
	// }

	switch (type) {
		case RESOURCE_FILE:
			result.action = ACTION_SERVE_FILE;
			result.targetPath = physicalPath;
			break;

		case RESOURCE_DIRECTORY:
			if (request.getRouteUri()[request.getRouteUri().length() - 1] != '/') {
				result.action = ACTION_REDIRECT;
				result.targetPath = request.getRouteUri() + "/";
				result.statusCode = MOVED_PERMANENTLY;
			} 
			else if (route && route->isAutoindex()) {
				result.action = ACTION_AUTOINDEX;
				result.targetPath = physicalPath;
			}
			else {
				result.action = ACTION_ERROR;
				result.statusCode = FORBIDDEN;
			}
			break;

		case RESOURCE_FORBIDDEN:
			result.action = ACTION_ERROR;
			result.statusCode = FORBIDDEN;
			break;

		case RESOURCE_NOT_FOUND:
		default:
			// std::cout << "resource not found\n";
			result.action = ACTION_ERROR;
			result.statusCode = NOT_FOUND;
			break;
	}

	return result;
}

/**
 * @brief Matches a list of URI segments to the most specific RouteConfig in the server tree.
 * * Traverses the `m_route_tree` using the pre-tokenized URI segments. Keeps 
 * track of the deepest node that contains a valid configuration to implement 
 * longest-prefix matching.
 * @param uriSegments A vector of path segments (e.g., {"api", "v1", "users"}).
 * @param server The target server configuration containing the routing tree.
 * @return A pointer to the most specific RouteConfig, or the server's default config if no match.
 */
const RouteConfig* RouteManager::matchRoute(const std::vector<std::string>& uriSegments, const Server* server, std::string& location) const {
	if (!server)
		return (NULL);
	const RouteNode* currNode = &(server->m_route_tree);
	const RouteConfig* bestMatch = server;
	// std::cout << "SERVER ROOT: " << server->getRoot() << std::endl;

	// std::cout << "server found " << server->getRedirection().second << "\n";
	if (currNode->config)
		bestMatch = currNode->config;

	for (std::vector<std::string>::const_iterator it = uriSegments.begin(); it != uriSegments.end(); ++it) {
		// std::cout << "\nsegment " << *it << "\n";
		std::map<std::string, RouteNode*>::const_iterator match = currNode->children.find(*it);
		if (match != currNode->children.end()) {
			// std::cout << "here " << currNode->config->getRedirection().second << "\n";
			// std::cout << "i find that " << *it << "\n";
			currNode = match->second;

			location += "/" + *it;
			// std::cout << "here " << currNode->config->getRedirection().second << "\n";
			if (currNode->config)
			{
				// std::cout << "============MATCH===============\n";
				bestMatch = currNode->config;
				// std::cout << "best match " << bestMatch->getRedirection().second << "\n";
			}
		} else {
			break;
		}
	}
	// std::cout << "this is important " << bestMatch->getRoot() << " LOCATION: " << location << "\n";

	return (bestMatch);
}

// const RouteConfig* RouteManager::matchRoute(const std::vector<std::string>& uriSegments, const Server* server) const {
//     if (!server)
//         return (NULL);
//     const RouteNode* currNode = &(server->m_route_tree);
//     const RouteConfig* bestMatch = server;

//     if (currNode->config)
//         bestMatch = currNode->config;

//     for (std::vector<std::string>::const_iterator it = uriSegments.begin(); it != uriSegments.end(); ++it) {

//         std::map<std::string, RouteNode*>::const_iterator match = currNode->children.find(*it);
//         if (match != currNode->children.end()) {;
//             currNode = match->second;

//             if (currNode->config && (it + 1) == uriSegments.end())
// 			{
//                 bestMatch = currNode->config;
// 			}
//         } else {
//             break;
//         }
//     }

//     return (bestMatch);
// }
