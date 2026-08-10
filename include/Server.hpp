#ifndef _SERVER_H
#define _SERVER_H

#include "webserver.hpp"
#include "RouteConfig.hpp"
#include "Location.hpp"


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
		void parseErrorPage(ContIter &begin);
		static HandlerFunc getDirectiveHandler(const std::string dir_name);

		const std::vector<std::string>& getServerNames(void) const;

		bool conflictsWith(const Server& other, std::string& server_name) const;
		void buildRouteTree();
		const std::vector<IPort>& getAddrs(void) const;
		bool hasServerName(const std::string& name) const;
		void setDefaultIport();
		~Server();


		typedef Location LocationType ;
		typedef ParseConfig ParseConfigType ;

		std::vector<LocationType> m_locations;
		RouteNode m_route_tree;
		std::string getErrorPage(HttpStatus) const;
	private:	
		void parseIPortV4(IPort& iport, ContIter& begin);
		void parseIPortV6(IPort& iport, ContIter& begin);
		std::vector<IPort> m_addr;
		std::vector<std::string> m_hosts;
		std::map<int, std::string> m_error_pages;
};

std::ostream& operator<<(std::ostream& out, const Server::IPort& iport);

typedef Server ServerType;

std::string getFamilyStr(const int family);
#include "IPort.hpp"
#endif
