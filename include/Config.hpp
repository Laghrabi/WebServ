#ifndef _CONFIG_H
#define _CONFIG_H

#include "webserver.hpp"

struct Config {
	typedef std::list<Server> ServerCont;

	typedef UnorderedMultiMap<Server::IPort, Server> ServerMultiMap;
	typedef ServerMultiMap::const_iterator ServerMultiMapConstIter;
	typedef std::pair<ServerMultiMapConstIter, ServerMultiMapConstIter> ServerRange;
	

	MimeTypesExt m_types;
	ServerMultiMap m_iport_server;
	std::list<Server> m_servers;
};


void print(const Config&conf);

#endif
