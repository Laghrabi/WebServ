#ifndef _SERVER_H
#define _SERVER_H

#include "webserver.hpp"
#include "RouteConfig.hpp"
#include "Location.hpp"
#include <sys/socket.h>

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
    RouteNode(const std::string& name) : segmentName(name), config(NULL) {}

	/**
     * @brief Recursively deletes the tree to ensure no memory leaks.
     * C++98 compliant deletion iterator for safely freeing all allocated child nodes.
     */
    ~RouteNode() {
        for (std::map<std::string, RouteNode*>::iterator it = children.begin(); it != children.end(); ++it) {
            delete it->second;
        }
    }
};

class Server : public RouteConfig {
	protected:
		typedef std::vector<token> Container;
		typedef Container::const_iterator ContIter;

	public:
		typedef void (Server::*HandlerFunc)(ContIter&);
	protected:

		typedef std::map<std::string, HandlerFunc> MapHandler ;
		static MapHandler s_handlers;

		static in_port_t default_port;
		static in_port_t default_ip;

	public:	
		static void init();
		struct IPort {
			public:
			int m_famlily;
			std::size_t m_size;
			IPort(int family, std::size_t size);

			IPort();
			IPort(const IPort& other);
			const sockaddr	*get() const;
			virtual void print() const;
			virtual bool operator==(const IPort& other) const;
			addrinfo getAddrHints() const;
			IPort& operator=(const Server::IPort& other);
			virtual ~IPort();
			protected:
			sockaddr *m_addr;
		};

		struct ParseIPortInterface {
			virtual void setIp(const std::string& ip) = 0;
			virtual void setPort(const std::string& port) = 0;
		};

		struct IPortV4;
		struct IPortV6;

		Server();
		void parseServerName(ContIter &begin);
		void parseIPort(ContIter &begin);
		static HandlerFunc getDirectiveHandler(const std::string dir_name);
		bool conflictsWith(const Server& other, std::string& server_name) const;
		void buildRouteTree();
		~Server();

		typedef Location LocationType ;
		typedef ParseConfig ParseConfigType ;

		std::vector<std::string> m_hosts;
		std::vector<IPort> m_addr;
		std::vector<LocationType> m_locations;
		RouteNode* m_route_tree;
};

std::ostream& operator<<(std::ostream& out, const Server::IPort& iport);
std::ostream& operator<<(std::ostream& out, const sockaddr_in6& addr);
std::ostream& operator<<(std::ostream& out, const sockaddr_in& addr);
std::ostream& operator<<(std::ostream& out, const Server::IPort& iport);

typedef Server ServerType;

#include "IPortV4.hpp"
#include "IPortV6.hpp"
#endif
