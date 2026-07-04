#ifndef _CONFIG_H
#define _CONFIG_H

#include "webserver.hpp"

struct Config {
	typedef std::list<Server> ServerCont;

	// NOTE: typedef for iport server map to ease the code
	typedef UnorderedMultiMap<Server::IPort, Server> ServerMultiMap;

	MimeTypesExt m_types;
	ServerMultiMap m_iport_server;
	std::list<Server> m_servers;
};


void print(const Config&conf);

#endif
