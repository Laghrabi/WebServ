#ifndef _SERVER_H
#define _SERVER_H

#include "webserver.hpp"
#include <exception>
#include "ParseConfig.hpp"



typedef std::string dir_t;
// namespace 

template <typename Container> class Server {

	public:

		static in_port_t default_port;
		static in_port_t default_ip;

		typedef typename Container::iterator contIter;

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





		void parseAccessLog(contIter &begin, const contIter& end) {
			(void)begin;(void)end;
			// check if it is valid
			m_access_location = (*begin).value;
			std::cout << "access location = " << m_access_location << "\n";
			++begin;
		}


		void parseServerName(contIter &begin, const contIter& end) {
			if ((*begin).type != WORD)
				throw (std::runtime_error("eror no value"));
			while (begin != end && (*begin).type == WORD) {
				m_host.insert((*begin).value);
				std::cout << "server name = " << (*begin).value << "\n";
				begin++;
			}
		}

		void parseIndex(contIter &begin, const contIter& end) {
			(void)begin;
			(void)end;
		}

		void parseIPort(contIter &begin, const contIter& end) {
			std::string iport_str = (*begin).value;
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
				throw (ParseConfig<TokenCont>::ConfigExcept("duplcate iport", (*begin).line));
			}
			else {
				std::cout << (std::find(m_addr.begin(), m_addr.end(), iport) == m_addr.end()) << "\n";
			}
			m_addr.push_back(iport);
			++begin;
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

		std::vector<IPort> m_addr;

		std::list<std::string> m_indexes;


};

// Server::IPort parseIPort(std::string iport);
#endif
