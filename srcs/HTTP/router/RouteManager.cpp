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

std::string toPath(std::vector<std::string>::const_iterator begin, const std::vector<std::string>::const_iterator end, bool is_dir) {
	std::string result;
	for (; begin != end; ++begin) {
		result += "/" + *begin;
	}
	if (is_dir)
		result += "/";
	return result;
}

bool RouteManager::isCgi(const std::vector<std::string>& script_path, RouteResult &result, const std::string& location) const{

	typedef std::vector<std::string> UriCont;
	typedef UriCont::const_iterator UriContConstIter;

	std::cout << "================\nlocation : " << location << "\n";
	std::string test_path = location;
	FileStatus status;
	std::string extention;

	UriContConstIter it = script_path.begin();
	for (; it != script_path.end(); ++it) {
		test_path += "/" + *it;
		std::cout << "[CGI] test path is " << test_path << "\n";
		result.cgiInfo.scriptName += "/" + *it;
		status.set(test_path);
		if (status.exist()) {
			if (status.isDir()) {
#ifdef CGI_DEBUG
				std::cout << "[(CGI) " << test_path << "is a directory\n";
#endif
				continue ;
			}
			else if (result.route->isCgiScript(*it)) {
				result.action = ACTION_EXECUTE_CGI;
				result.targetPath = test_path;
				result.statusCode = OK;
				result.cgiInfo.pathInfo = toPath(++it, script_path.end(), false);
				
				std::cout << "[CGI i found it ext = " << test_path << "]" << "\n";
				std::cout << "[CGI path info = " << result.cgiInfo.pathInfo << "\n";
				return (true);
			}
			else {
				return (false);
			}
		}
		else {
			std::cout << "[(CGI) path: " << test_path << "is not found ]";
			break;
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
void RouteManager::processRequest(HttpRequest& request) {
	RouteResult& result = request._routeResult;
	result.statusCode = OK;
	result.route = matchRoute(request.getUriSegments(), request.getServer(), _basePath);
	std::string	LocationMatch = _basePath;

	if (result.route->hasMaxBodySize()) {
		if (request._bodyBytesWritten > result.route->getMaxBodySize()) {
			result.statusCode = PAYLOAD_TOO_LARGE;
			result.action = ACTION_ERROR;
			return ;
		}
	}

	if (result.route && !result.route->isAllowed(request.getMethod())) {
		result.action = ACTION_ERROR;
		result.statusCode = METHOD_NOT_ALLOWED;
		return ;
	}

	if (result.route->doesRedirect()) {
		result.action = ACTION_REDIRECT;
		result.targetPath = result.route->getRedirection().second;
		result.statusCode = result.route->getRedirection().first;
		return ;
	}

	
	std::string physicalPath = _locator.buildPhysicalPath(request, _basePath, _resource);
	if (physicalPath.empty()) {
		result.action = ACTION_ERROR;
		result.statusCode = NOT_FOUND;
	}

	
	if (result.route->isCgiEnable()) {
		std::vector<std::string> vec;
		HttpRequest::normalizeUriHelper(_resource, vec);
		if (isCgi(vec, result, _basePath)) {
			return ;
		}
	}

	if (request.getMethod() == "POST") {
		if (result.route->getUploadDir().empty()) {
			result.action = ACTION_ERROR;
			result.statusCode = FORBIDDEN;
			return ;
		}
		std::string uploadsPath = resolveUploadPath(request.getRouteUri(), LocationMatch, result.route->getUploadDir());
		result.action = ACTION_UPLOAD_FILE;
		result.targetPath = uploadsPath;
		return ;
	}

	physicalPath = _locator.resolvePath(physicalPath, result.route);
	ResourceType type = _locator.getResourceType(physicalPath);
	determineResourceAction(result, type, physicalPath, request.getRouteUri());

	return ;
}

void RouteManager::determineResourceAction(RouteResult& result, ResourceType type, const std::string& physicalPath, const std::string& routeUri)  {
    switch (type) {
        case RESOURCE_FILE:
            result.action = ACTION_SERVE_FILE;
            result.targetPath = physicalPath;
            break;

        case RESOURCE_DIRECTORY:
            // Check for missing trailing slash
            if (routeUri[routeUri.length() - 1] != '/') {
                result.action = ACTION_REDIRECT;
                result.targetPath = routeUri + "/";
                result.statusCode = MOVED_PERMANENTLY;
            } 
            else if (result.route && result.route->isAutoindex()) {
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
            result.action = ACTION_ERROR;
            result.statusCode = NOT_FOUND;
            break;
    }
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
	const RouteConfig* RouteManager::matchRoute(const std::vector<std::string>& uriSegments, const Server* server, std::string& location){
		if (!server)
			return (NULL);
		const RouteNode* currNode = &(server->m_route_tree);
		const RouteConfig* bestMatch = server;
		// std::cout << "SERVER ROOT: " << server->getRoot() << std::endl;

		// std::cout << "server found " << server->getRedirection().second << "\n";
		if (currNode->config)
			bestMatch = currNode->config;

		for (std::vector<std::string>::const_iterator it = uriSegments.begin(); it != uriSegments.end(); ++it) {
			// std::cout << "hello\n";
			std::string segment = *it;
			std::cout << "\nsegment " << *it << "\n";
			std::cout << "SIZE= " << uriSegments.size() << std::endl;
			std::map<std::string, RouteNode*>::const_iterator match = currNode->children.find(*it);
			if (match != currNode->children.end()) {
				std::cout << "i find that " << *it << "\n";
				// std::cout << "here " << currNode->config->getRedirection().second << "\n";
				currNode = match->second;
				if ((!location.empty() && location[location.length() - 1] == '/') ||
					(location.empty() && segment == "/"))
						location += segment;
				else
					location += "/" + segment;
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

	return (bestMatch);
}

std::string RouteManager::resolveUploadPath(const std::string& uri, const std::string& locationMatch, const std::string& uploadStore) {
    std::string remainder = "";

    if (uri.find(locationMatch) == 0) {
        remainder = uri.substr(locationMatch.length());
    } else {
        size_t lastSlash = uri.find_last_of('/');
        if (lastSlash != std::string::npos) {
            remainder = uri.substr(lastSlash + 1);
        } else {
            remainder = uri;
        }
    }

    std::string finalPath = uploadStore;
    
    bool storeEndsWithSlash = (!finalPath.empty() && finalPath[finalPath.length() - 1] == '/');
    bool remainderStartsWithSlash = (!remainder.empty() && remainder[0] == '/');

    if (storeEndsWithSlash && remainderStartsWithSlash) {
        finalPath += remainder.substr(1);
    } 
    else if (!storeEndsWithSlash && !remainderStartsWithSlash && !remainder.empty()) {
        finalPath += "/" + remainder;
    } 
    else {
        finalPath += remainder;
    }

    return (finalPath);
}

void RouteManager::printRouteAction(RouteAction action) {
    switch (action) {
        case ACTION_SERVE_FILE:
            std::cout << "Action: Serving static file.\n";
            break;
        case ACTION_SERVE_INDEX:
            std::cout << "Action: Serving directory index file.\n";
            break;
        case ACTION_AUTOINDEX:
            std::cout << "Action: Generating autoindex directory listing.\n";
            break;
        case ACTION_EXECUTE_CGI:
            std::cout << "Action: Executing CGI script.\n";
            break;
        case ACTION_REDIRECT:
            std::cout << "Action: Performing HTTP redirection.\n";
            break;
        case ACTION_ERROR:
            std::cout << "Action: Handling route error state.\n";
            break;
        case NONE:
            std::cout << "Action: No action specified.\n";
            break;
        default:
            std::cout << "Action: Unknown route action.\n";
            break;
    }
}
