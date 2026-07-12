#include "../../../include/webserver.hpp"

RouteManager::RouteManager() : _locator() {}

RouteManager::RouteManager(const RouteManager& other) : _locator(other._locator) {}

RouteManager& RouteManager::operator=(const RouteManager& other) {
    if (this != &other) {
        _locator = other._locator;
    }
    return *this;
}

RouteManager::~RouteManager() {}

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
    const RouteConfig* route = matchRoute(request.getUriSegments(), request.getServer());

    if (route && !route->isAllowed(request.getMethod())) {
        result.action = ACTION_ERROR;
        result.statusCode = METHOD_NOT_ALLOWED;
        return result;
    }

    if (route && !route->getRedirectUrl().empty()) {
        result.action = ACTION_REDIRECT;
        result.targetPath = route->getRedirectUrl();
        result.statusCode = route->getRedirectStatusCode();
        return result;
    }

    std::string physicalPath = _locator.resolvePath(request.getRouteUri(), route, request.getServer());
    ResourceType type = _locator.getResourceType(physicalPath);

    if (type == RESOURCE_FILE && _locator.isCgiExtension(physicalPath)) {
        if (route && route->isCgiEnabled()) {
            result.action = ACTION_EXECUTE_CGI;
            result.targetPath = physicalPath;
            return result;
        }
    }

    switch (type) {
        case RESOURCE_FILE:
            result.action = ACTION_SERVE_FILE;
            result.targetPath = physicalPath;
            break;

        case RESOURCE_DIRECTORY:
            if (request.getRouteUri()[request.getRouteUri().length() - 1] != '/') {
                result.action = ACTION_REDIRECT;
                result.targetPath = request.getRouteUri() + "/";
                result.statusCode = route->getRedirectStatusCode();;
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
const RouteConfig* RouteManager::matchRoute(const std::vector<std::string>& uriSegments, const Server* server) const {
    if (!server)
        return (NULL);
    const RouteNode* currNode = &(server->m_route_tree);
    const RouteConfig* bestMatch = server;

    if (currNode->config)
        bestMatch = currNode->config;
        
    for (std::vector<std::string>::const_iterator it = uriSegments.begin(); it != uriSegments.end(); ++it) {
        std::map<std::string, RouteNode*>::const_iterator match = currNode->children.find(*it);
        if (match != currNode->children.end()) {
            currNode = match->second;
            if (currNode->config)
                bestMatch = currNode->config;
        } else {
            break;
        }
    }

    return (bestMatch);
}
