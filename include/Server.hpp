#ifndef _SERVER_H
#define _SERVER_H

#include "webserver.hpp"
#include "RouteConfig.hpp"
#include "Location.hpp"

/**
 * @brief A node within the routing tree representing a single URI segment.
 * * Each node corresponds to a specific directory or segment in a URL path (e.g., "api", "v1").
 * The tree structure allows for fast, segment-by-segment matching of incoming HTTP requests
 * to their corresponding location configurations.
 */
struct RouteNode {
	std::string segmentName;
	RouteConfig* config; 
	std::map<std::string, RouteNode*> children;

	/**
	 * @brief Constructs a new Route Node.
	 * @param name The URI segment string this node represents.
	 */
	RouteNode(const std::string& name) : segmentName(name){
		config = new RouteConfig;
	}

	/**
	 * @brief Copy constructor for RouteNode, performing a deep copy of the tree.
	 * * Deeply copies the `segmentName`, clones the `RouteConfig` (if present),
	 * and recursively clones all child nodes to ensure the new tree is completely 
	 * independent of the original.
	 * @param other The RouteNode instance to copy.
	 */
	RouteNode(const RouteNode& other){
		// std::cout << "RouteConfig copy constructor\n" << std::endl;
		segmentName = other.segmentName;
		config = new RouteConfig;
		*config = *other.config;
		for (std::map<std::string, RouteNode*>::const_iterator it = other.children.begin(); 
				it != other.children.end(); ++it) {
			this->children[it->first] = new RouteNode(*(it->second));
		}
	}

	RouteNode& operator=(const RouteNode& other) {

		// std::cout << "RouteConfig | copy constructor\n" << std::endl;
		delete config;
		for (std::map<std::string, RouteNode*>::iterator it = children.begin(); it != children.end(); ++it) {
			delete it->second;
		}

		segmentName = other.segmentName;
		config = new RouteConfig;
		*config = *other.config;

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
	~RouteNode() {
		// std::cout << "delete\n" << std::endl;
		delete config;
		for (std::map<std::string, RouteNode*>::iterator it = children.begin(); it != children.end(); ++it) {
			delete it->second;
		}
	}
};

class Server : public RouteConfig {
	public:
		typedef void (Server::*HandlerFunc)(ContIter&);

	protected:
		typedef std::map<std::string, HandlerFunc> MapHandler ;
		static MapHandler s_handlers;

		static in_port_t default_port;
		static in_port_t default_ip;

	public:	
		static void init();

		struct IPort;

		struct ParseIPortInterface {
			virtual bool isStrictIp(const std::string& ip) = 0;
			virtual void setIp(const std::string& ip) = 0;
			virtual void setPort(const std::string& port) = 0;
			protected:
			virtual void setIpString() = 0;
			virtual void setPortString() = 0;
		};

		struct IPortV4;
		struct IPortV6;

		Server();
		Server(const Server&);
		Server& operator=(const Server&);

		void parseServerName(ContIter &begin);
		void parseIPort(ContIter &begin);
		static HandlerFunc getDirectiveHandler(const std::string dir_name);

		const std::vector<std::string>& getServerNames(void) const;

		bool conflictsWith(const Server& other, std::string& server_name) const;
		void buildRouteTree();
		const std::vector<IPort>& getAddrs(void) const;
		bool hasServerName(const std::string& name) const;
		~Server();


		typedef Location LocationType ;
		typedef ParseConfig ParseConfigType ;

		std::vector<LocationType> m_locations;
		RouteNode m_route_tree;
	private:	
		std::vector<IPort> m_addr;
		std::vector<std::string> m_hosts;
};

std::ostream& operator<<(std::ostream& out, const Server::IPort& iport);

typedef Server ServerType;

std::string getFamilyStr(const int family);
#include "IPort.hpp"
#endif
