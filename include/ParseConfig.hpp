#ifndef _PARSECONFIG_H
#define _PARSECONFIG_H

#include "tokenization.hpp"
#include "webserver.hpp"
#include "Config.hpp"
#include "Server.hpp"

template <typename Container> class ParseConfig {
	protected:

		typedef Server<Container> ServerType;
		typedef typename Container::iterator ContIter;


	public:

		typedef void (ServerType::*serverDirHandler)(ContIter&, const ContIter&);

		std::vector<token>::iterator m_it;
		std::vector<token>::iterator m_end;
		std::map<std::string, serverDirHandler> m_dir_handler;

		ParseConfig(Container& tokens) :
			m_it(tokens.begin()),
			m_end(tokens.end())
	{
		m_dir_handler["access_log"] = &ServerType::parseAccessLog;
		m_dir_handler["server_name"] = &ServerType::parseServerName;
		m_dir_handler["listen"] = &ServerType::parseIPort;
	}

		serverDirHandler getServerDirhandler(std::string &directive_name) {
			if (m_dir_handler.find(directive_name) == m_dir_handler.end())
				return (NULL);
			return (m_dir_handler[directive_name]);
		}


		void parseSimpleDir() {
		}

		Config parse(void) {
			Config config;

			for (; m_it != m_end; ++m_it) {
				while (true) {
					ServerType server;
					if ((*m_it).is("server")) {
						std::cout << "i found a new server\n";
						++m_it;
						if ((*m_it).is(END_OF_FILE)) {
							//  here if we have server ant then nothing
							throw (ConfigExcept("got server directive witout body", (*m_it).line));
						}
						try {
							if (!(*m_it).is(OPEN_BRACE)) {
								throw (ConfigExcept("expected OPEN PRACE '{' after server directive and got "
											+ (*m_it).value, (*m_it).line));
							parseServer(server);
							}
						}
						catch(const ParseConfig<Container>::ConfigExcept& e) {
							std::cerr << e.what()	 << "\n";
						}
						m_config.m_servers.push_back(server);
					}
					else {
						break ;
					}
				}
			}
			return (config);
		}

		void parseServer(ServerType& server) {

			++m_it;
			if ((*m_it).is_eof()) {
				throw (ConfigExcept("expected CLOSE PRACE after '}' server directive", (*m_it).line));
			}
			// while (!(*m_it).is_eof() && (*m_it).type != CLOSE_BRACE)
			// {

			std::cout << (*m_it).value << "\n";
			parseServerDir(server);
			// }
			if ((*m_it).is_eof()) {
				throw (std::runtime_error("expect CLOSE BRACE '}' after server directive"));
			}
			else if (!(*m_it).is(CLOSE_BRACE)) {
				throw (std::runtime_error("expect CLOSE BRACE '}' after server directive got " + (*m_it).value), (*m_it).line);
			}
			++m_it;
		}

		void parseServerDir(ServerType& server) {
			while (true) {
				if (!(*m_it).is_eof() && (*m_it).is(WORD)) {
					std::cout << "there is a simple server directive \n";
					parseServerSimpleDir(server);
				}
				else {
					std::cout << "break\n";
					std::cout << "hey[" << (*m_it).value << "]\n";
					break ;
				}
			}
		}

		void parseServerSimpleDir(ServerType& server) {
			std::string		directive_name = (*m_it).value;
			serverDirHandler handler = getServerDirhandler(directive_name);

			if (handler == NULL) {
				throw (std::runtime_error("unsopported server directive name"));
			}
			++m_it;
			if ((*m_it).is_eof()) {
				throw (std::runtime_error("expected server simple directive value"));
			}

			(server.*handler)(m_it, m_end);
			if (!(*m_it).is(SEMICOLON)) {
				throw (std::runtime_error("server simple directive needs SEMICOLON ';' in the end. got " + (*m_it).value));
			}
			++m_it;
		}

		Config m_config;
		int m_line;

		class ConfigExcept : std::runtime_error
	{
		private:
			std::string msg;
		public:
			ConfigExcept(std::string message, int line) : runtime_error(message) {
				std::stringstream ss;
				ss << line;
				msg = message + " in line " + ss.str();
			}
			const char *what() const throw() {
				return (msg.c_str());
			}
			~ConfigExcept() throw() {}

	};
};

#endif
