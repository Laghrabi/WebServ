#ifndef ROUTE_NODE_HPP
#define ROUTE_NODE_HPP

/**
 * @brief A node within the routing tree representing a single URI segment.
 * * Each node corresponds to a specific directory or segment in a URL path (e.g., "api", "v1").
 * The tree structure allows for fast, segment-by-segment matching of incoming HTTP requests
 * to their corresponding location configurations.
 */
struct RouteNode {
	std::string segmentName;
	Location* config; 
	std::map<std::string, RouteNode*> children;

	RouteNode(const std::string& name);
	RouteNode(const RouteNode& other);
	RouteNode& operator=(const RouteNode& other);
	~RouteNode();
};

#endif