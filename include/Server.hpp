#ifndef _SERVER_H
#define _SERVER_H

#include "webserver.hpp"
#include <exception>
#include "ParseConfig.hpp"

template <typename Container = std::vector<token> > class Location {
	public:
		typedef typename Container::iterator ContIter;

		typedef void (Location::*HandlerFunc)(ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler ;
		static MapHandler s_handlers;

		static HandlerFunc getDirectiveHandler(const std::string dir_name) {
			if (s_handlers.find(dir_name) == s_handlers.end()) {
				return (NULL);
			}
			return (s_handlers[dir_name]);
		}

		static void init(void) {
			if (s_handlers.empty()) {
				s_handlers["index"] = &Location::parseIndex;
				s_handlers["root"] = &Location::parseRoot;
				s_handlers["upload_dir"] = &Location::parseUploadDir;
				s_handlers["access_log"] = &Location::parseAccessLog;
			}
		}
		std::string m_location;
		std::string m_root;
		std::string m_upload_dir;
		std::string m_access_log;
		std::list<std::string> m_indexes;
		bool m_autoindex;

	public:
		Location() {
			init();
		}


		void parseAutoIndex(ContIter &begin) {
			
			if (!begin->is("on") && !begin->is("off")) {
				throw (ParseConfig<TokenCont>::ConfigExcept("autoindex simple directive expect on or off, unexpected '" + begin->value + "'", begin->line));
			}
			m_autoindex = begin->value == "on" ? true : false;
			++begin;
		}

		void parseIndex(ContIter &begin) {
			while (begin->is(WORD)) {
				m_indexes.push_back(begin->value);
				++begin;
			}
		}

		void parseRoot(ContIter &begin) {
			m_root = begin->value;
			++begin;
		}

		void parseLocation(ContIter &begin) {
			std::string location = begin->value;
			// check location
			begin++;
			m_location = location;

			(void)begin;
			
		}
		void parseUploadDir(ContIter &begin) {
			// check if upload dir is valid
			m_upload_dir = begin->value;	
			++begin;
		}

		void parseAccessLog(ContIter &begin) {
			m_access_log = begin->value;
			std::cout << "access location = " << m_access_log << "\n";
			++begin;
		}
};

template<typename T> typename Location<T>::MapHandler Location<T>::s_handlers;

typedef std::string dir_t;
// namespace 

template <typename Container = std::vector<token> > class Server : public Location<Container> {

	public:

		typedef typename Container::iterator ContIter;
		typedef void (Server::*HandlerFunc)(ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler ;
		static MapHandler s_handlers;
		static void init() {
			if (s_handlers.empty()) {
				s_handlers["access_log"] = &Server::parseAccessLog;
				s_handlers["server_name"] = &Server::parseServerName;
				s_handlers["listen"] = &Server::parseIPort;
				s_handlers["autoindex"] = &Server::parseAutoIndex;	
				s_handlers["index"] = &Server::parseIndex;
				s_handlers["root"] = &Server::parseRoot;
				s_handlers["upload_dir"] = &Server::parseUploadDir;
			}
		}

		static in_port_t default_port;
		static in_port_t default_ip;


		struct IPort {
			IPort(in_addr_t addr, in_port_t port): m_addr_ip(addr),
			m_port(port) {}
			IPort(){}
			bool operator==(const IPort& other) {
				return (m_addr_ip == other.m_addr_ip && m_port == other.m_port);
			}
			void setIp(const std::string& ip) throw(std::exception) {
				struct in_addr addr;
				int sucess;

				sucess = inet_pton(AF_INET, ip.c_str(), &addr);
				if (sucess == 1) {
					m_addr_ip = addr.s_addr;	
				}
				else {
					if (sucess == 0)	
					{
						throw (std::exception());
					}
					else {
						perror("error inet_pton");
						throw (std::exception());
					}
				}
			}
			void setPort(const std::string& port) throw(std::exception) {
				std::stringstream ss(port);
				ss >> m_port;
				if (ss.fail())
					throw (std::exception());
			}


			void setIp(in_addr_t ip) {
				m_addr_ip = ip;
			}


			private:
			in_addr_t m_addr_ip;	
			in_port_t m_port;
		};
		Server(){}







		void parseServerName(ContIter &begin) {
			while (begin->is(WORD)) {
				m_host.insert(begin->value);
				std::cout << "server name = " << begin->value << "\n";
				begin++;
			}
		}




		void parseIPort(ContIter &begin) {
			std::string iport_str = begin->value;
			size_t pos;
			IPort iport(0, 80);

			std::string ip;
			std::string port;

			if ((pos = iport_str.find(":")) != std::string::npos) {
				ip = iport_str.substr(0, pos);
				port = iport_str.substr(pos + 1);

				iport.setPort(ip);
				iport.setPort(port);

				std::cout << ip << "\n";
				std::cout << "\n";
				std::cout << port << "\n";

			}
			else if (iport_str.find(".") != std::string::npos) {
				iport.setIp(iport_str);
			}
			else {
				iport.setPort(iport_str);
			}
			if (std::find(m_addr.begin(), m_addr.end(), iport) != m_addr.end())
			{
				std::cout << "hey found duplacate";
				throw (ParseConfig<TokenCont>::ConfigExcept("duplcate iport", begin->line));
			}
			else {
				std::cout << (std::find(m_addr.begin(), m_addr.end(), iport) == m_addr.end()) << "\n";
			}
			m_addr.push_back(iport);
			++begin;
		}

		static HandlerFunc getDirectiveHandler(const std::string dir_name) {
			if (s_handlers.find(dir_name) == s_handlers.end()) {
				return (NULL);
			}
			return (s_handlers[dir_name]);
		}

	public:


		std::string m_root;
		std::set<std::string> m_host;
		std::string m_upload_dir;

		std::vector<IPort> m_addr;

		std::list<std::string> m_indexes;

		std::vector<Location<Container> > m_locations;



};

template<typename T> typename Server<T>::MapHandler Server<T>::s_handlers;

#endif
