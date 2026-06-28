#ifndef _CONFIG_H
#define _CONFIG_H

#include "webserver.hpp"
#include "MimeTypesExt.hpp"
#include "Server.hpp"
#include "UnorderedMultiMap.hpp"

struct Config {
	typedef std::list<Server> ServerCont;
	MimeTypesExt m_types;
	UnorderedMultiMap<Server::IPort, Server> m_iport_server;
	std::list<Server> m_servers;
};


void print(const Config&conf);

#endif
