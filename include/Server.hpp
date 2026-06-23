#ifndef _SERVER_H
#define _SERVER_H

#include "webserver.hpp"
#include "Location.hpp"

typedef std::string dir_t;
// namespace 

class Server : public Location {

	public:
		typedef std::vector<token> Container;
		typedef Container::iterator ContIter;
		typedef void (Server::*HandlerFunc)(ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler ;
		static MapHandler s_handlers;

		static void init();

		static in_port_t default_port;
		static in_port_t default_ip;

		struct IPort {

			IPort();
			IPort(in_addr_t addr, in_port_t port);

			bool operator==(const IPort& other) const;
			void setIp(const std::string& ip) throw(std::exception);
			void setIp(in_addr_t ip);

			void setPort(const std::string& port) throw(std::exception);

			const in_port_t& getPort() const;
			const in_addr_t& getAddr() const;

			bool operator<(const IPort& other) const;

			private:
			in_addr_t m_addr_ip;
			in_port_t m_port;
		};

		Server();
		void parseServerName(ContIter &begin);
		void parseIPort(ContIter &begin);
		static HandlerFunc getDirectiveHandler(const std::string dir_name);
		bool conflictsWith(const Server& other, std::string& server_name) const;
		void make_pair(std::multimap<Server::IPort, const Server*>& iport_server_map) const;

		typedef Location LocationType ;
		typedef ParseConfig ParseConfigType ;

		std::vector<std::string> m_hosts;
		std::set<std::string> m_ordered_hosts;
		std::vector<IPort> m_addr;
		std::set<IPort> m_ordered_addr;
		std::vector<LocationType> m_locations;
};

std::ostream& operator<<(std::ostream& out, const Server::IPort& iport);

typedef Server ServerType;

#endif
