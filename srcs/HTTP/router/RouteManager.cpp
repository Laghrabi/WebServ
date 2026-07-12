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
    const RouteConfig* route = matchRoute(request.getRouteUri(), request.getServer());

    if (route && !route->isAllowed(request.getMethod())) {
        result.action = ACTION_ERROR;
        result.statusCode = METHOD_NOT_ALLOWED;
        return result;
    }

    if (route && !route->getRedirectUrl().empty()) {
        result.action = ACTION_REDIRECT;
        result.targetPath = route->getRedirectUrl();
        result.statusCode = route->getRedirectStatusCode();;
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
 * @brief Matches a request URI to the most specific RouteConfig using the Trie tree.
 * * Traverses the server's routing tree segment-by-segment. Continues traversal as 
 * long as matching segments exist, keeping track of the deepest node that holds 
 * a configuration (longest prefix matching).
 * @param uri The normalized URI path from the request.
 * @param server The server configuration to search within.
 * @return A pointer to the best-matching RouteConfig, or NULL if the server is invalid.
 */
const RouteConfig* RouteManager::matchRoute(const std::string& uri, const Server* server) const {
    if (!server)
        return (NULL);
    const RouteNode* currNode = &(server->m_route_tree);
    const RouteConfig* bestMatch = server;

    if (currNode->config)
        bestMatch = currNode->config;
    
    size_t start = 0;
    size_t end = 0;
    while (start < uri.length()) {
        end = uri.find('/', start);
        if (end == std::string::npos)
            end = uri.length();
        std::string segment = uri.substr(start, end - start);
        start = end + 1;
        if (segment.empty())
            continue ;
        
        std::map<std::string, RouteNode*>::const_iterator it = currNode->children.find(segment);
        if (it != currNode->children.end()) {
            currNode = it->second;
            if (currNode->config)
                bestMatch = currNode->config;
        }
        else
            break ;
    }
    return (bestMatch);
}
