#include "../../../include/webserver.hpp"

/**
 * @brief Default constructor for HttpRouter.
 */
HttpRouter::HttpRouter() {}

/**
 * @brief Copy constructor for HttpRouter.
 */
HttpRouter::HttpRouter(const HttpRouter& other) {
    *this = other;
}

/**
 * @brief Assignment operator for HttpRouter.
 */
HttpRouter& HttpRouter::operator=(const HttpRouter& other) {
    (void)other;
    return (*this);
}

/**
 * @brief Destructor for HttpRouter.
 */
HttpRouter::~HttpRouter() {}

/**
 * @brief Matches a request URI to the most specific RouteConfig in the server tree.
 * * Traverses the `m_route_tree` segment-by-segment based on the URI. 
 * Keeps track of the "best match" encountered (the deepest node with an 
 * associated configuration) to support hierarchical location inheritance.
 * @param uri The normalized URI path to match.
 * @param server The target server configuration containing the route tree.
 * @return A pointer to the most specific RouteConfig found, or NULL if the server is null.
 */
const RouteConfig* HttpRouter::matchRoute(const std::string& uri, const Server* server) const {
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
