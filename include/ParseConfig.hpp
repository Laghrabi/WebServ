#ifndef _PARSECONFIG_H
#define _PARSECONFIG_H

#include "tokenization.hpp"
#include "webserver.hpp"
#include "Config.hpp"
#include "Server.hpp"
#include <stdexcept>
#include <vector>

template <typename Container> class ParseConfig {
	protected:

		typedef Server<Container> ServerType;
		typedef typename Container::iterator ContIter;


	public:

		typedef void (ServerType::*serverDirHandler)(ContIter&, const ContIter&);
		std::vector<token>::iterator m_it;
		std::vector<token>::iterator m_end;
		std::map<std::string, serverDirHandler> m_dir_handler;

		// serverDirHandler getServerDirhandler(std::string &directive_name);


		serverDirHandler getServerDirhandler(std::string &directive_name) {
			if (m_dir_handler.find(directive_name) == m_dir_handler.end())
				return (NULL);
			return (m_dir_handler[directive_name]);
		}


		void parseSimpleDir(ServerType& server) {
			(void)server;
		};

		void parseServerSimpleDir(ServerType& server) {
			std::string		directive_name = (*m_it).value;
			serverDirHandler handler = getServerDirhandler(directive_name);
			if (handler == NULL) {
				throw (std::runtime_error("unsopported server directive name"));
			}
			++m_it;
			if (m_it == m_end)
			{
				throw (std::runtime_error("expected server simple directive value"));
			}
			
		
			(server.*handler)(m_it, m_end);
			if ((*m_it).type !=	SEMICOLON) {
				throw (std::runtime_error("server simple directive needs SEMICOLON ';' in the end."));
			}
			++m_it;
		}

		void parseServerDir(ServerType& server) {
			if (m_it == m_end) {
				throw (std::runtime_error("hey"));
			}
			else {
				while (true) {
					try {
						if ((*m_it).type == WORD) {
							std::cout << "tere is a simple server directive \n";
							parseServerSimpleDir(server);
						}
						else {
							std::cout << "brea\n";
							break ;
						}
					}
					catch (...)
					{
						throw ;
					}
				}
			}
		}

		void parseServer(ServerType& server) {
			if (m_it == m_end) {
				//  here if we have server ant then nothing
				throw (std::exception());
			}
			// if we have server then not open prace {
			if ((*m_it).type != OPEN_BRACE) {
				throw (std::runtime_error("expected OPEN PRACE after '{' server directive"));
			}
			else {
				m_it++;
				while ((*m_it).type != CLOSE_BRACE)
				{
					std::cout << (*m_it).value << "\n";
					try {
						parseServerDir(server);
					} catch (std::exception& e) {
						throw ;		
					}
				}
			}
		};
		ParseConfig(Container& tokens) :
			m_it(tokens.begin()),
			m_end(tokens.end())
			{
				m_dir_handler["access_log"] = &ServerType::parseAccessLog;
				m_dir_handler["server_name"] = &ServerType::parseServerName;
			}

		Config m_config;



		Config parse(void) {
			ServerType server;
			Config config;
			for (; m_it != m_end; m_it++) 
			{
				while (true) {
					if ((*m_it).value == "server")
					{
						std::cout << "i found a new server\n";
						m_it++;
						try {
							parseServer(server);
						} catch (std::exception& e) {
							throw ;	
						}
					}
					else {
						break ;
					}
				}
			}
			return (config);
		}


		class ConfigExcept : std::exception
			{

			};
		};

#endif
