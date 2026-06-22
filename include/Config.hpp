#ifndef _CONFIG_H
#define _CONFIG_H

#include "webserver.hpp"
#include "MimeTypesExt.hpp"

template <typename Container> class Server;

struct Config {
	typedef std::vector<Server<std::vector<token> > > ServerCont;
	MimeTypesExt m_types;
	std::vector<Server<std::vector<token> > > m_servers;
};


void print(const Config&conf);

#endif
