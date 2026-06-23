#ifndef _CONFIG_H
#define _CONFIG_H

#include "webserver.hpp"
#include "MimeTypesExt.hpp"
#include "Server.hpp"


struct Config {
	typedef std::list<Server> ServerCont;
	MimeTypesExt m_types;
	std::multimap<Server::IPort, const Server*> m_iport_server;
	std::list<Server> m_servers;
};


void print(const Config&conf);

#endif
