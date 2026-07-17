#include "webserver.hpp"

RouteConfig::MapHandler RouteConfig::s_handlers;
std::set<std::string> RouteConfig::s_available_methods;


RouteConfig::RouteConfig(const RouteConfig& other) :
	m_root(other.m_root),
	m_upload_dir(other.m_upload_dir),
	m_access_log(other.m_access_log),
	m_indexes(other.m_indexes),
	m_autoindex(other.m_autoindex),
	m_max_body_size(other.m_max_body_size),
	m_max_body_size_exist(other.m_max_body_size_exist),
	m_allowed_methods(other.m_allowed_methods),
	m_cgi_map(other.m_cgi_map),
	m_redirect(other.m_redirect),
	m_does_redirect(other.m_does_redirect)
{

}

RouteConfig& RouteConfig::operator=(const RouteConfig& other) {

	m_root = other.m_root;
	m_upload_dir = other.m_upload_dir;
	m_access_log = other.m_access_log;
	m_indexes = other.m_indexes;
	m_autoindex = other.m_autoindex;
	m_max_body_size = other.m_max_body_size;
	m_max_body_size_exist = other.m_max_body_size_exist;
	m_allowed_methods = other.m_allowed_methods;
	m_cgi_map = other.m_cgi_map;
	m_redirect = other.m_redirect;
	m_does_redirect = other.m_does_redirect;

	return (*this);
}

void RouteConfig::init(void) {
	if (s_handlers.empty()) {
		s_handlers["index"] = &RouteConfig::parseIndex;
		s_handlers["root"] = &RouteConfig::parseRoot;
		s_handlers["upload_dir"] = &RouteConfig::parseUploadDir;
		s_handlers["access_log"] = &RouteConfig::parseAccessLog;
		s_handlers["max_client_body_size"] = &RouteConfig::parseMaxBodySize;
		s_handlers["allowed_methods"] = &RouteConfig::parseAllowedMethods;
		s_handlers["cgi"] = &RouteConfig::parseCgiConf;
		s_handlers["redirect"] = &RouteConfig::parseRedirection;
	}
}


RouteConfig::HandlerFunc RouteConfig::getDirectiveHandler(const std::string dir_name) {
	if (s_handlers.find(dir_name) == s_handlers.end()) {
		return (NULL);
	}
	return (s_handlers[dir_name]);
}

bool RouteConfig::notRedirectCode(int code) const {
	return (code >= 300 && code <= 308);
}

void RouteConfig::parseRedirection(ContIter& begin) {
	std::stringstream ss;
	ss << begin->value;
	ss >> m_redirect.first;
	if (notRedirectCode(m_redirect.first)) {
		throw (ParseConfig::ConfigExcept("invalid redirect code", begin->line));
	}
	++begin;

	if (begin->is_eof()) {
		throw (ParseConfig::ConfigExcept("got code but nor url", begin->line));
	}
	m_redirect.second = begin->value;
	m_does_redirect = true;
	++begin;
}

bool RouteConfig::doesRedirect(void) const {
	return (m_does_redirect);
}

void RouteConfig::initAvailableMethods() {
	s_available_methods.insert("GET");
	s_available_methods.insert("POST");
	s_available_methods.insert("DELETE");
}

RouteConfig::RouteConfig() : 
	m_autoindex(false),
	m_max_body_size_exist(false),
	m_does_redirect(false){
		initAvailableMethods();
		init();
	}

bool RouteConfig::validExtention(const std::string& ext, std::string& err_msg) {
	if (ext[0] != '.')
	{
		err_msg = "extention is not valid: because it does not begin with .";
		return (false);
	}
	if (ext.find('.', 1) != std::string::npos) {
		err_msg = "extention is not valid: because it has more than a .";
		return (false);
	}
	if (mapElemExist(m_cgi_map, ext)) {
		err_msg = "duplicate extention: " + ext;
		return (false);
	}
	return (true);
}

void RouteConfig::parseCgiConf(ContIter &begin) {
	std::string ext;
	std::string err_msg;
	while (begin->is(WORD))	{
		ext = begin->value;
		if (!validExtention(ext, err_msg)) {
			throw (ParseConfig::ConfigExcept(err_msg, begin->line));
		}
		m_cgi_map.insert(ext);
		++begin;
	}
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

const std::pair<int, std::string>& RouteConfig::getRedirection() const{
	return (m_redirect);
}

bool RouteConfig::isAllowed(const std::string& method) const{
	return (m_allowed_methods.find(method) != m_allowed_methods.end());
}


const std::string& RouteConfig::getRoot() const {
	return m_root;
}

const std::string& RouteConfig::getUploadDir() const {
	return m_upload_dir;
}

const std::string& RouteConfig::getAccessLog() const {
	return m_access_log;
}

const std::list<std::string>& RouteConfig::getIndexes() const{
	return m_indexes;
}

bool RouteConfig::isAutoindex() const{
	return m_autoindex;
}

std::size_t RouteConfig::getMaxBodySize() const
{
	return m_max_body_size;
}

bool RouteConfig::hasMaxBodySize() const
{
	return m_max_body_size_exist;
}

RouteConfig::~RouteConfig() {

}
