#ifndef _PARSECONFIG_H
#define _PARSECONFIG_H

#include "webserver.hpp"
#include "Config.hpp"
#include "Server.hpp"

class ParseConfig {
	protected:

		void parseServer(Server& server);
		void parseServerDir(Server& server);
		void parseSimpleDir(Server& server);
		void parseServerSimpleDir(Server& server);

	public:

		std::vector<token>::iterator m_it;
		std::vector<token>::iterator m_end;
		Config m_config;

		Config parse(void);

		ParseConfig(std::vector<token>& tokens);

		class ConfigExcept : std::exception
	{

	};
};

#endif
