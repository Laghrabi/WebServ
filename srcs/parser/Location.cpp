#include "RouteConfig.hpp"
#include "webserver.hpp"
#include "Location.hpp"

Location::MapHandler Location::s_handlers;

Location::Location() : RouteConfig() {
	std::cout << "s_handlers = " << s_handlers.size() << "\n";
}

void Location::init() {
	for (RouteConfig::MapHandler::const_iterator it = RouteConfig::s_handlers.begin(); it != RouteConfig::s_handlers.end(); ++it) {
		s_handlers[it->first] = it->second;
	}
}


Location::~Location(){}
