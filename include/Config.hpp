#ifndef _CONFIG_H
#define _CONFIG_H

#include "webserver.hpp"
#include "MimeTypesExt.hpp"

template <typename Container> class Server;

struct Config {
	MimeTypesExt m_types;
	std::list<Server<std::vector<token> > > m_servers;
};

#endif
