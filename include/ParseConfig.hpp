#ifndef _PARSECONFIG_H
#define _PARSECONFIG_H

#include "tokenization.hpp"
#include "webserver.hpp"
#include "Config.hpp"
#include "MimeTypesExt.hpp"
#include "tokenization.hpp"

class ParseConfig {

	public:
		// types
		typedef Server ServerType;
		typedef Location LocationType;
		typedef lexer::Container Container;
		typedef lexer::ContIter ContIter;

	private:
		Config m_config;
		ContIter m_it;


	public:

		template <typename T> void parseContext(T &context, void (ParseConfig::*func)(T&), std::string context_name);
		ParseConfig(Container& tokens);

		template <typename T> void parseServerSimpleDir(T& context, const std::string& name);

		void parseAllMimeTypes(MimeTypesExt& mime_types);
		bool checkServerConflict(Config::ServerCont::const_iterator first, Config::ServerCont::const_iterator last,	const ServerType& value, std::string& server_name);
		Config parse(void);

		void parseServerLocation(LocationType& location);
		
		void make_pair(const Server& server);

		void parseServer(ServerType& server);

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
