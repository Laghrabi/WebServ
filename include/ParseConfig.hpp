#ifndef _PARSECONFIG_H
#define _PARSECONFIG_H

#include "tokenization.hpp"
#include "webserver.hpp"
#include "Config.hpp"


template <typename Container> class Server;
template <typename Container> class Location;


template <typename Container> class ParseConfig {
	protected:

		typedef Server<Container> ServerType;
		typedef Location<Container> LocationType;
		typedef typename Container::iterator ContIter;


	public:

		typedef void (ServerType::*serverDirHandler)(ContIter&, const ContIter&);
		typedef void (LocationType::*LocationDirHandler)(ContIter&, const ContIter&);

		std::vector<token>::iterator m_it;
		std::vector<token>::iterator m_end;
		std::map<std::string, serverDirHandler> m_dir_handler;
		std::map<std::string, LocationDirHandler> m_location_handler;
		

		ParseConfig(Container& tokens) :
			m_it(tokens.begin()),
			m_end(tokens.end())
	{
		m_dir_handler["access_log"] = &ServerType::parseAccessLog;
		m_dir_handler["server_name"] = &ServerType::parseServerName;
		m_dir_handler["listen"] = &ServerType::parseIPort;
		m_dir_handler["autoindex"] = &ServerType::parseAutoIndex;

		m_location_handler["index"] = &LocationType::parseIndex;
		m_location_handler["root"] = &LocationType::parseRoot;
		m_location_handler["upload_dir"] = &LocationType::parseUploadDir;

	}

		template <typename T> typename T::HandlerFunc getServerDirHandler(std::string &directive_name) {
			// const typename T::MapHandler& map_handler = T::s_handlers;
			std::map<std::string, typename T::HandlerFunc> map_handler = T::s_handlers;
			if (map_handler.find(directive_name) == map_handler.end())
				return (NULL);
			return (map_handler[directive_name]);
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
						if (!(*m_it).is(OPEN_BRACE)) {
							throw (ConfigExcept("expected OPEN PRACE '{' after server directive and got "
										+ (*m_it).value, (*m_it).line));
						}
						parseServer(server);
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
				throw (ParseConfig<TokenCont>::ConfigExcept("expect CLOSE BRACE '}' after server directive", (*m_it).line));
			}
			else if (!(*m_it).is(CLOSE_BRACE)) {
				throw (ParseConfig<TokenCont>::ConfigExcept("expect CLOSE BRACE '}' after server directive got " + (*m_it).value, (*m_it).line));
			}
			++m_it;
		}

		void parseServerLocation(LocationType& location) {
(void)location;
			++m_it;
			if ((*m_it).is(END_OF_FILE)) {
				//  here if we have server ant then nothing
				throw (ConfigExcept("got location directive witout body", (*m_it).line));
			}
			if (!(*m_it).is(OPEN_BRACE)) {
				throw (ConfigExcept("expected OPEN PRACE '{' after location directive and got "
							+ (*m_it).value, (*m_it).line));
			}
			// parseServer(server);
			// m_config.m_servers.push_back(server);
			;
		}

		void parseServerDir(ServerType& server) {

			std::string		directive_name;
			LocationType location;

			while (true) {
				if (!(*m_it).is_eof() && (*m_it).is(WORD)) {
					std::cout << "there is a simple server directive \n";
					directive_name = (*m_it).value;
					if (directive_name == "location") {
						parseServerLocation(location);
					}
					else {
						parseServerSimpleDir<ServerType>(server, "server");
					}
				}
				else {
					break ;
				}
			}
		}

		template <typename T> void parseServerSimpleDir(T& context, std::string name) {
			std::string		directive_name = (*m_it).value;
			// else {

			typename T::HandlerFunc handler = getServerDirHandler<ServerType>(directive_name);

			if (handler == NULL) {
				throw (ParseConfig<TokenCont>::ConfigExcept("unsopported " + name + "directive name", (*m_it).line));
			}
			++m_it;
			if ((*m_it).is_eof()) {
				throw (ParseConfig<TokenCont>::ConfigExcept("expected " + name + " simple directive value", (*m_it).line));
			}

			(context.*handler)(m_it, m_end);
			if (!(*m_it).is(SEMICOLON)) {
				throw (ParseConfig<TokenCont>::ConfigExcept("server simple directive needs SEMICOLON ';' in the end. got " + (*m_it).value, (*m_it).line));
			}
			++m_it;
			// }
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


typedef ParseConfig<TokenCont> ParseConfigVec ;

#endif
