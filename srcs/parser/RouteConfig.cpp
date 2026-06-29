#include "tokenization.hpp"
#include "webserver.hpp"
#include "RouteConfig.hpp"
#include "ParseConfig.hpp"

RouteConfig::MapHandler RouteConfig::s_handlers;
std::set<std::string> RouteConfig::s_available_methods;

void RouteConfig::init(void) {
	if (s_handlers.empty()) {
		s_handlers["index"] = &RouteConfig::parseIndex;
		s_handlers["root"] = &RouteConfig::parseRoot;
		s_handlers["upload_dir"] = &RouteConfig::parseUploadDir;
		s_handlers["access_log"] = &RouteConfig::parseAccessLog;
		s_handlers["max_client_body_size"] = &RouteConfig::parseMaxBodySize;
		s_handlers["allowed_methods"] = &RouteConfig::parseAllowedMethods;
	}
}


RouteConfig::HandlerFunc RouteConfig::getDirectiveHandler(const std::string dir_name) {
	if (s_handlers.find(dir_name) == s_handlers.end()) {
		return (NULL);
	}
	return (s_handlers[dir_name]);
}


void RouteConfig::initAvailableMethods() {
	s_available_methods.insert("GET");
	s_available_methods.insert("POST");
	s_available_methods.insert("DELETE");
}

RouteConfig::RouteConfig() : m_max_body_size_exist(false) {
	initAvailableMethods();
	init();
}


void RouteConfig::parseAutoIndex(ContIter &begin) {
	if (!begin->is("on") && !begin->is("off")) {
		throw (ParseConfigType::ConfigExcept("autoindex simple directive expect on or off, unexpected '" + begin->value + "'", begin->line));
	}
	m_autoindex = begin->value == "on" ? true : false;
	++begin;
}

void RouteConfig::parseIndex(ContIter &begin) {
	while (begin->is(WORD)) {
		m_indexes.push_back(begin->value);
		++begin;
	}
}

void RouteConfig::parseRoot(ContIter &begin) {
	m_root = begin->value;
	++begin;
}

void RouteConfig::parseMaxBodySize(ContIter &begin) {
	std::stringstream ss;
	ss << begin->value;
	// std::size_t size;
	ss >> m_max_body_size;
	m_max_body_size_exist = true;
	++begin;
}

//
// void RouteConfig::parseRouteConfig(ContIter &begin) {
// 	std::string location = begin->value;
// 	// check location
// 	begin++;
// 	m_location = location;
// }

void RouteConfig::parseUploadDir(ContIter &begin) {
	// check if upload dir is valid
	m_upload_dir = begin->value;	
	++begin;
}

void RouteConfig::parseAccessLog(ContIter &begin) {
			m_access_log = begin->value;
			++begin;
}


void RouteConfig::parseAllowedMethods(ContIter &begin) {
	std::string err_msg;
	while (begin->is(WORD)) {
		try {
			addMethod(begin->value);
		} catch (const std::exception& e) {
			err_msg = e.what();
			throw (ParseConfig::ConfigExcept(err_msg, begin->line));
		}
		++begin;
	}
}


void RouteConfig::addMethod(const std::string& method) throw (std::exception) {
	if (s_available_methods.find(method) == s_available_methods.end()) {
		throw (std::runtime_error("aknown method, not supported method: " + method));
	}
	if (m_allowed_methods.find(method) != m_allowed_methods.end()) {
		throw (std::runtime_error("duplicate method"));
	}
	m_allowed_methods.insert(method);
}


bool RouteConfig::isAllowed(const std::string& method) {
	return (m_allowed_methods.find(method) != m_allowed_methods.end());
}

