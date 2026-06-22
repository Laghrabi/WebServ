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

		template <typename T> void parseServerSimpleDir(T& context, const std::string& name) {
			const std::string&		directive_name = m_it->value;
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

		bool checkServerConflict(typename Config::ServerCont::const_iterator first, typename Config::ServerCont::const_iterator last,	const ServerType& value, std::string& server_name) {
			for (; first != last ; first++) {
				if (value.conflictsWith(*first, server_name))
				{
					return (true);
				}
			}
			return (false);
		}

		Config parse(void) {
				int server_begin_line;
				while (true) {
					ServerType server;
					if (m_it->is("server")) {
						server_begin_line = m_it->line;
						parseContext(server, &ParseConfig::parseServer, "server");
						std::string server_name;
						if (checkServerConflict(m_config.m_servers.begin(), m_config.m_servers.end(), server, server_name))
							throw (ParseConfig<TokenCont>::ConfigExcept("conflict Server Name '" + server_name + "'", server_begin_line));
						m_config.m_servers.push_back(server);
					}
					else if (m_it->is("types")) {
						parseContext(m_config.m_types, &ParseConfig::parseAllMimeTypes, "types")	;
					}
					else {
						break ;
					}
				}
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
