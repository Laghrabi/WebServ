#include "webserver.hpp"

Location::MapHandler Location::s_handlers;

Location::Location() : RouteConfig() {
	// std::cout << "s_handlers = " << s_handlers.size() << "\n";
}


void Location::init() {
	for (RouteConfig::MapHandler::const_iterator it = RouteConfig::s_handlers.begin();
			it != RouteConfig::s_handlers.end(); ++it) {
		s_handlers[it->first] = it->second;
	}
}

bool Location::hasSamePath(const Location& location) {
	return (m_path == location.m_path);
}


const std::string& Location::getPath(void) const {
	return (m_path);
}

Location::~Location(){}
