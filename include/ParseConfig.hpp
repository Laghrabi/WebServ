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

		typedef void (ServerType::*serverDirHandler)(ContIter&, const ContIter&);
		typedef void (LocationType::*LocationDirHandler)(ContIter&, const ContIter&);

		std::vector<token>::iterator m_it;
		std::vector<token>::iterator m_end;


		ParseConfig(Container& tokens) :
			m_it(tokens.begin()),
			m_end(tokens.end())
	{
		LocationType::init();
		ServerType::init();
	}


void callHandler(MimeTypesExt& hey,std::string ) {
	(hey.parseMimeType)(m_it, m_end);
}


		template <typename T> void callHandler(T context, std::string &directive_name) {
			const typename T::MapHandler& map_handler = T::s_handlers;

			// if (map_handler.find(directive_name) == map_handler.end())
				// return (NULL);
			(context.*(*map_handler.find(directive_name)))(m_it, m_end);
			// return (map_handler.at(directive_name));
		}

		/*

NOTE:
if type is not found content type will be:
application/octet-stream, Raw binary data (unknown type)

*/
		void parseAllMimeTypes(MimeTypesExt& mime_types) {
			while ((*m_it).is(WORD)) {
				parseServerSimpleDir(mime_types, "");
			}
			++m_it;
		}
// }

// void parseMimeType(std::map<std::string, std::string> mime_map) {
// 	std::string type = (*m_it).value;
// 	++m_it;
// 	while ((*m_it).is(WORD)) {
// 		std::string ext = (*m_it).value;
// 		if (mime_map.find(ext) != mime_map.end())
// 		{
// 			throw (ParseConfig<>::ConfigExcept("duplacate extention: " + ext, (*m_it).line));
// 		}
// 		mime_map[ext] = type;
// 		++m_it;
// 	}
// }


// void parseSimpleDir() {
// }

template <typename T> void parseContext(T context, void (ParseConfig::*func)(T&), std::string context_name) {
	++m_it;
	if ((*m_it).is(END_OF_FILE)) {
		throw (ConfigExcept("got" + context_name + " context witout body", (*m_it).line));
	}
	if (!(*m_it).is(OPEN_BRACE)) {
		throw (ConfigExcept("expected OPEN PRACE '{' after" + context_name + "context and got "
					+ (*m_it).value, (*m_it).line));
	}
	++m_it;
	(this->*func)(context);
	std::cout << (*m_it).value << "hey hey\n";
	if ((*m_it).is_eof() || !(*m_it).is(CLOSE_BRACE)) {
		std::string msg = "expect CLOSE BRACE '}' after " + context_name + " context";
		if (!(*m_it).is_eof())
			msg += "got " + (*m_it).value;
		throw (ParseConfig<TokenCont>::ConfigExcept(msg, (*m_it).line));
	}
	++m_it;
}

Config parse(void) {
	Config config;

	for (; m_it != m_end; ++m_it) {
		while (true) {
			ServerType server;
			if ((*m_it).is("server")) {
				std::cout << "i found a new server\n";
				parseContext(server, &ParseConfig::parseServer, "server");
				m_config.m_servers.push_back(server);
			}
			else if ((*m_it).is("types")) {
				std::cout << "i find types";
				parseContext(config.m_types, &ParseConfig::parseAllMimeTypes, "types")	;
			}
			else {
				break ;
			}
		}
	}
	return (config);
}

void parseServer(ServerType& server) {
	LocationType location;

	while ((*m_it).is(WORD)) {
		if ((*m_it).is("location")) {
			std::string path;
			++m_it;
			if ((*m_it).is_eof()) {
				throw (ConfigExcept("no path after location context", (*m_it).line));
			}
			path = (*m_it).value;
			std::cout << "path is " << path << "\n";
			parseContext(location, &ParseConfig::parseServerLocation, "location");
		}
		else {
			parseServerSimpleDir<ServerType>(server, "server");
		}
	}
}

void parseServerLocation(LocationType& location) {

	while ((*m_it).is(WORD)) {
		parseServerSimpleDir(location, "location");
	}

	std::cout << (*m_it).value << "but but\n";
	// m_it++;
}


template <typename T> void parseServerSimpleDir(T& context, std::string name) {
	std::string		directive_name = (*m_it).value;

	callHandler(context,directive_name);

	// if (handler == NULL) {
	// 	throw (ParseConfig<TokenCont>::ConfigExcept("unsopported " + name + " directive name: " + (*m_it).value, (*m_it).line));
	// }
	++m_it;
	if ((*m_it).is_eof()) {
		throw (ParseConfig<TokenCont>::ConfigExcept("expected " + name + " simple directive value", (*m_it).line));
	}
	// (context.*handler)(m_it, m_end);
	if (!(*m_it).is(SEMICOLON)) {
		throw (ParseConfig<TokenCont>::ConfigExcept("server simple directive needs SEMICOLON ';' in the end. got " + (*m_it).value, (*m_it).line));
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


typedef ParseConfig<TokenCont> ParseConfigVec ;

#endif
