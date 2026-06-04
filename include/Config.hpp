#ifndef _CONFIG_H
#define _CONFIG_H

#include "webserver.hpp"

template <typename Container> class Server;

struct Config {
	std::list<Server<std::vector<token> > > m_servers;
};

#endif
