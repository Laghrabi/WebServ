#ifndef _CONFIG_H
#define _CONFIG_H

#include "webserver.hpp"
#include "Server.hpp"

class Config {
	std::list<Server<std::vector<token> > > m_servers;
};

#endif
