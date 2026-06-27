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

	public:
		typedef std::vector<token> Container;
		typedef Container::iterator ContIter;
		typedef void (Server::*HandlerFunc)(ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler ;
		static MapHandler s_handlers;

		static void init();

		static in_port_t default_port;
		static in_port_t default_ip;

		// struct IPort {
		// 	IPort();
		// 	IPort(in_addr_t addr, in_port_t port);
		//
		// 	struct sockaddr addr;	
		// 	bool operator==(const IPort& other) const;
		// 	void setIp(const std::string& ip) throw(std::exception);
		// 	void setIp(in_addr_t ip);
		//
		// 	void setPort(const std::string& port) throw(std::exception);
		//
		// 	const in_port_t& getPort() const;
		// 	const in_addr_t& getAddr() const;
		//
		// 	bool operator<(const IPort& other) const;
		//
		// 	private:
		// 	in_addr_t m_addr_ip;
		// 	in_port_t m_port;
		// };

		struct IPort {
			public:
			const int m_famlily;
			const std::size_t m_size;
			IPort(int family, std::size_t size);

			virtual const sockaddr	*get() const;

			virtual void print() const;
			virtual bool operator==(const IPort& other) const;
			bool operator<(const IPort& other) const;
			addrinfo getAddrHints() const;
			virtual ~IPort();
			protected:
			sockaddr *m_addr;
		};

		struct ParseIPortInterface {
			virtual void setIp(const std::string& ip) = 0;
			virtual void setPort(const std::string& port) = 0;
		};

		struct IPortV4 : public IPort, public ParseIPortInterface{
			IPortV4();
			virtual void setIp(const std::string& ip);
			virtual void setPort(const std::string& port);
			// virtual void print() const;
			virtual bool operator==(const IPort& other) const;
			bool isStrictIp(const std::string& ip);
			private:
			sockaddr_in* m_addr;
		};


		struct IPortV6 : public IPort, public ParseIPortInterface{
			IPortV6();
			virtual void setIp(const std::string& ip);
			virtual void setPort(const std::string& port);
			// virtual void print() const;
			virtual bool operator==(const IPort& other) const;
			bool isStrictIp(const std::string& ip);
			private:
			sockaddr_in6* m_addr;
		};


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
		std::set<std::string> m_ordered_hosts;
		std::vector<IPort> m_addr;
		// std::set<IPort> m_ordered_addr;
		std::vector<LocationType> m_locations;
		RouteNode* m_route_tree;
};

std::ostream& operator<<(std::ostream& out, const Server::IPort& iport);

typedef Server ServerType;



#endif
