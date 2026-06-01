#ifndef _SERVER_H
#define _SERVER_H

#include "webserver.hpp"
#include <exception>

typedef std::string dir_t;
// namespace 

class Server {

	public:
		struct IPort {
			IPort(in_addr_t addr, in_port_t port): m_addr_ip(addr),
			m_port(port){
			}
			IPort(){}
			void setIp(const std::string& ip_str) throw(std::exception);
			void setPort(const std::string& port_str) throw(std::exception);

			void setIp(in_addr_t addr);
			void setPort(in_port_t port);

			private:
			in_addr_t m_addr_ip;	
			in_port_t m_port;
		};
		Server();

		struct ParseServer {
      static void parseAccessLog(Server &server, std::vector<token>::iterator &it);
      static void parseServerName(Server &server, std::vector<token>::iterator &it);
      static void parseIPort(Server &server, std::vector<token>::iterator &it);
      static void parseIndex(Server &server, std::vector<token>::iterator &it);
		};


	private:
		

		class Location {
			public:
				std::string m_location;
				std::list<std::string> m_indexes;
				std::string m_root;
				std::string m_upload_dir;
		};

    std::string m_access_location;
    std::string m_root;
		std::set<std::string> m_host;
		std::string m_upload_dir;

		std::list<IPort> m_addr;

		std::list<std::string> m_indexes;

    static in_port_t default_port;
    static in_port_t default_ip;
};

Server::IPort parseIPort(std::string iport);
#endif
