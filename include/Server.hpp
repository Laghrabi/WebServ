#ifndef _SERVER_H
#define _SERVER_H

#include "webserver.hpp"
#include "RouteConfig.hpp"
#include "Location.hpp"
#include <sys/socket.h>

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
			int m_famlily;
			std::size_t m_size;
			IPort(int family, std::size_t size);

			IPort();
			IPort(const IPort& other);

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
		~Server();

		typedef Location LocationType ;
		typedef ParseConfig ParseConfigType ;

		std::vector<std::string> m_hosts;
		std::set<std::string> m_ordered_hosts;
		std::vector<IPort> m_addr;
		// std::set<IPort> m_ordered_addr;
		std::vector<LocationType> m_locations;
};

std::ostream& operator<<(std::ostream& out, const Server::IPort& iport);

std::ostream& operator<<(std::ostream& out, const sockaddr_in6& addr);
std::ostream& operator<<(std::ostream& out, const sockaddr_in& addr);
std::ostream& operator<<(std::ostream& out, const Server::IPort& iport);

typedef Server ServerType;



#endif
