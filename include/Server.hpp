#ifndef _SERVER_H
#define _SERVER_H

#include "webserver.hpp"
#include <exception>

typedef std::string dir_t;
// namespace 

template <typename Container> class Server {

	public:
		typedef typename Container::iterator contIter;

		struct IPort {
			IPort(in_addr_t addr, in_port_t port): m_addr_ip(addr),
			m_port(port) {}
			IPort(){}
			void setIp(const std::string& ip_str) throw(std::exception);
			void setPort(const std::string& port_str) throw(std::exception);

			void setIp(in_addr_t addr);
			void setPort(in_port_t port);

			private:
			in_addr_t m_addr_ip;	
			in_port_t m_port;
		};
		Server(){}

		void parseIPort(contIter &begin, const contIter& end);



		void parseAccessLog(contIter &begin, const contIter& end) {
			(void)begin;(void)end;
			// if ((*it).type != WORD)
			// {
			//   throw std::exception();
			// }
			// check if it is valid
			// server.m_access_location = (*it).value;
		}


		void parseServerName(contIter &begin, const contIter& end) {
			while (begin != end && (*begin).type == WORD) {
				m_host.insert((*begin).value);
				std::cout << (*begin).value << " hell o\n";
				begin++;
			}
		}

		// void Server::parseIPort(std::vector<token>::iterator &it) {
		// }



		void parseIndex(contIter &begin, const contIter& end) {
			(void)begin;
			(void)end;
		}

		// void check(Server &server, std::vector<token>::iterator &it, void (*func)(std::string& str)) {
		//   while ((*it).type != WORD) {
		//     // check if it is valid
		//     // (server.m_indexes).push_back((*it).value);
		//     func((*it).value);
		//     it++;
		//   }
		// }


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

// Server::IPort parseIPort(std::string iport);
#endif
