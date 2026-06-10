#ifndef _PARSECONFIG_H
#define _PARSECONFIG_H

#include "tokenization.hpp"
#include "webserver.hpp"
#include "Config.hpp"
#include "MimeTypesExt.hpp"

template <typename Container> class Server;
template <typename Container> class Location;
struct MimeTypesExt;

template <typename Container = std::vector<token> > class ParseConfig {
	protected:

		typedef Server<Container> ServerType;
		typedef Location<Container> LocationType;
		typedef typename Container::iterator ContIter;


	public:

		std::vector<token>::iterator m_it;


		ParseConfig(Container& tokens) :
			m_it(tokens.begin())
	{
		LocationType::init();
		ServerType::init();
	}

		template <typename T> void parseServerSimpleDir(T& context, std::string name) {
			std::string		directive_name = m_it->value;
			std::string err_msg;

			typename T::HandlerFunc handler = T::getDirectiveHandler(directive_name);
			if (handler == NULL) {
				throw (ParseConfig<TokenCont>::ConfigExcept("unknown " + name + " directive name: " + m_it->value, m_it->line));
			}
			++m_it;
			if (m_it->is_eof() || !m_it->is(WORD)) {
				err_msg = "";
				throw (ParseConfig<TokenCont>::ConfigExcept("expected " + name + " simple directive value", m_it->line));
			}
			(context.*handler)(m_it);
			if (m_it->is_eof() || !m_it->is(SEMICOLON)) {
				err_msg = "error message";
				if (!m_it->is_eof()) {
					err_msg += " " + m_it->value;
				}
				throw (ParseConfig<TokenCont>::ConfigExcept(err_msg, m_it->line));
			}
			++m_it;
		}

		/*

NOTE: 
if type is not found content type will be:
application/octet-stream, Raw binary data (unknown type)

*/
		void parseAllMimeTypes(MimeTypesExt& mime_types) {
			std::string type;
			std::string err_msg;

			while (m_it->is(WORD)) {
				mime_types.parseMimeType(m_it);
				if (m_it->is_eof() || !m_it->is(SEMICOLON)) {
					err_msg = "error message";
					if (!m_it->is_eof()) {
						err_msg += "hey";
					}
					throw (ParseConfig<TokenCont>::ConfigExcept(err_msg, m_it->line));
				}
				++m_it;
			}
		}

		template <typename T> void parseContext(T &context, void (ParseConfig::*func)(T&), std::string context_name) {
			++m_it;
			if (m_it->is(END_OF_FILE)) {
				throw (ConfigExcept("got" + context_name + " context witout body", m_it->line));
			}
			if (!m_it->is(OPEN_BRACE)) {
				throw (ConfigExcept("expected OPEN PRACE '{' after" + context_name + "context and got "
							+ m_it->value, m_it->line));
			}
			++m_it;
			(this->*func)(context);
			if (m_it->is_eof() || !m_it->is(CLOSE_BRACE)) {
				std::string msg = "expect CLOSE BRACE '}' after " + context_name + " context";
				if (!m_it->is_eof())
					msg += "got " + m_it->value;
				throw (ParseConfig<TokenCont>::ConfigExcept(msg, m_it->line));
			}
			++m_it;
		}

		Config parse(void) {
			std::cout << m_it->value << "hello\n";

			// for (; m_it != m_end; ++m_it) {
				while (true) {
					ServerType server;
					if (m_it->is("server")) {
						std::cout << "i found a new server\n";
						parseContext(server, &ParseConfig::parseServer, "server");
						m_config.m_servers.push_back(server);
					}
					else if (m_it->is("types")) {
						std::cout << "i find types";
						parseContext(m_config.m_types, &ParseConfig::parseAllMimeTypes, "types")	;
					}
					else {
						break ;
					}
				}
			// }
			std::cout << "\nhello " << m_config.m_types.empty() << "\n";
			return (m_config);
		}

		void parseServer(ServerType& server) {
			LocationType location;

			while (m_it->is(WORD)) {
				if (m_it->is("location")) {
					std::string path;
					++m_it;
					if (m_it->is_eof()) {
						throw (ConfigExcept("no path after location context", m_it->line));
					}
					path = m_it->value;
					std::cout << "path is " << path << "\n";
					parseContext(location, &ParseConfig::parseServerLocation, "location");
					server.m_locations.push_back(location);
				}
				else {
					parseServerSimpleDir<ServerType>(server, "server");
				}
			}
		}

		void parseServerLocation(LocationType& location) {

			while (m_it->is(WORD)) {
				parseServerSimpleDir(location, "location");
			}

			std::cout << m_it->value << "but but\n";
			// m_it++;
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
