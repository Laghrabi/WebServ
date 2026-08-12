#include "webserver.hpp"

/**
	 * @brief Constructs a new Route Node.
	 * @param name The URI segment string this node represents.
	 */
	RouteNode::RouteNode(const std::string& name) : segmentName(name), config(NULL) {}

	/**
	 * @brief Copy constructor for RouteNode, performing a deep copy of the tree.
	 * * Deeply copies the `segmentName`, clones the `Location` (if present),
	 * and recursively clones all child nodes to ensure the new tree is completely 
	 * independent of the original.
	 * @param other The RouteNode instance to copy.
	 */
	RouteNode::RouteNode(const RouteNode& other) : config(NULL) {
		segmentName = other.segmentName;
		if (other.config) {
			config = new Location;
			*config = *other.config;
		}
		for (std::map<std::string, RouteNode*>::const_iterator it = other.children.begin(); 
				it != other.children.end(); ++it) {
			this->children[it->first] = new RouteNode(*(it->second));
		}
	}

	RouteNode& RouteNode::operator=(const RouteNode& other) {
		delete config;
		for (std::map<std::string, RouteNode*>::iterator it = children.begin(); it != children.end(); ++it) {
			delete it->second;
		}

		segmentName = other.segmentName;
		if (other.config) {
			config = new Location;
			*config = *other.config;
		}

		for (std::map<std::string, RouteNode*>::const_iterator it = other.children.begin(); 
				it != other.children.end(); ++it) {
			this->children[it->first] = new RouteNode(*(it->second));
		}
		return (*this);
	}

	/**
	 * @brief Recursively deletes the tree to ensure no memory leaks.
	 * C++98 compliant deletion iterator for safely freeing all allocated child nodes.
	 */
	RouteNode::~RouteNode() {
		delete config;
		for (std::map<std::string, RouteNode*>::iterator it = children.begin(); it != children.end(); ++it) {
			delete it->second;
		}
	}
