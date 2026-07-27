#include "findElem.hpp"
#include "webserver.hpp"

Location::MapHandler Location::s_handlers;

Location::Location() : RouteConfig() {
	init();
	// std::cout << "s_handlers = " << s_handlers.size() << "\n";
}


void Location::init() {
	if (s_handlers.empty()) {
		RouteConfig::init();
		for (RouteConfig::MapHandler::const_iterator it = RouteConfig::s_handlers.begin();
				it != RouteConfig::s_handlers.end(); ++it) {
			s_handlers[it->first] = it->second;
		}
		s_handlers["alias"] = &Location::parseAlias;
	}
}

bool Location::hasSamePath(const Location& location) {
	return (m_path == location.m_path);
}

void Location::parseAlias(ContIter &begin) {
	m_alias = begin->value;
	++begin;
}

const std::string& Location::getAlias(void) const {
	return (m_alias);
}


Location::HandlerFunc Location::getDirectiveHandler(const std::string dir_name) {
	init();
	MapHandler::const_iterator it = s_handlers.find(dir_name);
	if (it != s_handlers.end()) {
		return (it->second);
	}
	return (NULL);
}


void Location::setPath(const std::string& path) {
	m_path = path;
}


template <typename T> void Location::copyDirectiveInfo(T& var, const T& new_val) {
	if (var.empty()) {
		if (!new_val.empty())
			var = new_val;
	}
}


void Location::copyServerRouteConfig(const RouteConfig& route_conf) {
	copyDirectiveInfo(m_root, route_conf.getRoot());
	copyDirectiveInfo(m_upload_dir, route_conf.getUploadDir());
	copyDirectiveInfo(m_access_log, route_conf.getAccessLog());
	copyDirectiveInfo(m_indexes, route_conf.getIndexes());
	if (!m_autoindex.first) {
		m_autoindex = route_conf.getAutoIndex();
	}
	if (!m_max_body_size.first) {
		m_max_body_size.first = route_conf.hasMaxBodySize();
		m_max_body_size.second = route_conf.getMaxBodySize();
	}
	copyDirectiveInfo(m_allowed_methods, route_conf.getAllowedMethods());
	copyDirectiveInfo(m_cgi_map, route_conf.getCgiMap());
}


const std::string& Location::getPath(void) const {
	return (m_path);
}

Location::~Location(){}
