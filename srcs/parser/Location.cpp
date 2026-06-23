#include "webserver.hpp"
#include "Location.hpp"
#include "ParseConfig.hpp"

Location::MapHandler Location::s_handlers;

void Location::init(void) {
	if (s_handlers.empty()) {
		s_handlers["index"] = &Location::parseIndex;
		s_handlers["root"] = &Location::parseRoot;
		s_handlers["upload_dir"] = &Location::parseUploadDir;
		s_handlers["access_log"] = &Location::parseAccessLog;
	}
}


Location::HandlerFunc Location::getDirectiveHandler(const std::string dir_name) {
	if (s_handlers.find(dir_name) == s_handlers.end()) {
		return (NULL);
	}
	return (s_handlers[dir_name]);
}


Location::Location() {
	init();
}


void Location::parseAutoIndex(ContIter &begin) {

	if (!begin->is("on") && !begin->is("off")) {
		throw (ParseConfigType::ConfigExcept("autoindex simple directive expect on or off, unexpected '" + begin->value + "'", begin->line));
	}
	m_autoindex = begin->value == "on" ? true : false;
	++begin;
}

void Location::parseIndex(ContIter &begin) {
	while (begin->is(WORD)) {
		m_indexes.push_back(begin->value);
		++begin;
	}
}


void Location::parseRoot(ContIter &begin) {
	m_root = begin->value;
	++begin;
}


void Location::parseLocation(ContIter &begin) {
	std::string location = begin->value;
	// check location
	begin++;
	m_location = location;
}

void Location::parseUploadDir(ContIter &begin) {
	// check if upload dir is valid
	m_upload_dir = begin->value;	
	++begin;
}

void Location::parseAccessLog(ContIter &begin) {
			m_access_log = begin->value;
			++begin;
}
