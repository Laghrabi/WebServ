#include "findElem.hpp"
#include "webserver.hpp"
#include <cstdlib>

RouteConfig::MapHandler RouteConfig::s_handlers;
std::set<std::string> RouteConfig::s_available_methods;
RouteConfig::UnitMap RouteConfig::m_unit_multi;

void RouteConfig::initUnitMultiplier()
{
    m_unit_multi['B'] = 1;
    m_unit_multi['K'] = 1024ULL;
    m_unit_multi['M'] = 1024ULL * 1024ULL;
    m_unit_multi['G'] = 1024ULL * 1024ULL * 1024ULL;
}

void RouteConfig::init(void) {
	if (s_handlers.empty()) {

		initUnitMultiplier();

		s_handlers["index"] = &RouteConfig::parseIndex;
		s_handlers["autoindex"] = &RouteConfig::parseAutoIndex;
		s_handlers["root"] = &RouteConfig::parseRoot;
		s_handlers["upload_dir"] = &RouteConfig::parseUploadDir;
		s_handlers["access_log"] = &RouteConfig::parseAccessLog;
		s_handlers["client_max_body_size"] = &RouteConfig::parseMaxBodySize;
		s_handlers["allowed_methods"] = &RouteConfig::parseAllowedMethods;
		s_handlers["cgi"] = &RouteConfig::parseCgiConf;
		s_handlers["redirect"] = &RouteConfig::parseRedirection;
	}
}

RouteConfig::RouteConfig(const RouteConfig& other)
	: m_root(other.m_root),
	m_upload_dir(other.m_upload_dir),
	m_access_log(other.m_access_log),
	m_indexes(other.m_indexes),
	m_autoindex(other.m_autoindex),
	m_max_body_size(other.m_max_body_size),
	m_allowed_methods(other.m_allowed_methods),
	m_cgi_map(other.m_cgi_map),
	m_redirect(other.m_redirect)
{
}

RouteConfig& RouteConfig::operator=(const RouteConfig& other)
{
	m_root = other.m_root;
	m_upload_dir = other.m_upload_dir;
	m_access_log = other.m_access_log;
	m_indexes = other.m_indexes;
	m_autoindex = other.m_autoindex;
	m_max_body_size = other.m_max_body_size;
	m_allowed_methods = other.m_allowed_methods;
	m_cgi_map = other.m_cgi_map;
	m_redirect = other.m_redirect;
	return *this;
}


RouteConfig::HandlerFunc RouteConfig::getDirectiveHandler(const std::string dir_name) {
	if (s_handlers.find(dir_name) == s_handlers.end()) {
		return (NULL);
	}
	return (s_handlers[dir_name]);
}

bool RouteConfig::RedirectCode(int code) const {
	return (code >= 300 && code <= 308);
}

void RouteConfig::parseRedirection(ContIter& begin) {
	// NOTE: do something here
	std::stringstream ss;
	ss << begin->value;
	int tmp; ss>>tmp;
	m_redirect.second.first = (HttpStatus)tmp;
	std::cout << "redirection " << m_redirect.first << "\n";
	if (!RedirectCode(m_redirect.second.first)) {
		throw (ParseConfig::ConfigExcept("invalid redirect code", begin->line));
	}
	++begin;

	if (begin->is_eof()) {
		throw (ParseConfig::ConfigExcept("got code but nor url", begin->line));
	}
	m_redirect.second.second = begin->value;
	m_redirect.first = true;
	++begin;
}

bool RouteConfig::doesRedirect(void) const {
	return (m_redirect.first);
}

void RouteConfig::initAvailableMethods() {
	s_available_methods.insert("GET");
	s_available_methods.insert("POST");
	s_available_methods.insert("DELETE");
}

RouteConfig::RouteConfig() {
		m_autoindex.first = false;
		m_max_body_size.first = false;
		m_redirect.first = false;
		initAvailableMethods();
		init();
	}

bool RouteConfig::validExtention(const std::string& ext, std::string& err_msg) {
	if (ext.empty()) {
		err_msg = "empty extention in cgi directive";
		return false;
	}
	if (ext[0] != '.')
	{
		err_msg = "extention is not valid: because it does not begin with .";
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
	m_autoindex.second = begin->value == "on" ? true : false;
	m_autoindex.first = true;
	++begin;
}

void RouteConfig::parseIndex(ContIter &begin) {
	// check empty string
	while (begin->is(WORD)) {
		if (begin->value.empty())
			throw (ParseConfigType::ConfigExcept("empty index", begin->line));
		m_indexes.push_back(begin->value);
		++begin;
	}
}

void RouteConfig::parseRoot(ContIter &begin) {
	m_root = begin->value;
	++begin;
}

/* NOTE: this function takes an iterator to token {value, type}
 * and expect [num][letter {M, G, K, B}] ex: 10M, 1000G witout point not a float
 */
void RouteConfig::parseMaxBodySize(ContIter &begin) {
	std::string numUnit = begin->value;
	if (numUnit.find(" \t") != std::string::npos) {
		throw (ParseConfigType::ConfigExcept("max body size malformed", begin->line));
	}
	std::stringstream ss(numUnit);
	if (!(ss >> m_max_body_size.second))
		throw (ParseConfigType::ConfigExcept("max body size malformed", begin->line));
	m_max_body_size.first = true;
	std::string unitChar;
	if (!(ss >> unitChar) || unitChar.size() != 1)
		throw (ParseConfigType::ConfigExcept("", begin->line));
	UnitMap::iterator it = m_unit_multi.find(unitChar.at(0));
	if (it == m_unit_multi.end())
	{
		throw (ParseConfigType::ConfigExcept("max body size malformed, " + unitChar
					+ " not a supporeted unit", begin->line));
	}
	m_max_body_size.second *= it->second;
	++begin;
}


void RouteConfig::parseUploadDir(ContIter &begin) {
	// TODO: check if upload dir is valid
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
		if (begin->value.empty())
			throw (ParseConfig::ConfigExcept("empty allowed_methods value", begin->line));
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

const std::pair<HttpStatus, std::string>& RouteConfig::getRedirection() const{
	return (m_redirect.second);
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

const std::pair<bool, bool>& RouteConfig::getAutoIndex() const {
	return (m_autoindex);
}

bool RouteConfig::isAutoindex() const{
	return m_autoindex.second;
}


std::size_t RouteConfig::getMaxBodySize() const {
	return m_max_body_size.second;
}

bool RouteConfig::hasMaxBodySize() const
{
	return m_max_body_size.first;
}

const std::set<std::string>& RouteConfig::getAllowedMethods() const {
	return m_allowed_methods;
}

const std::set<std::string>& RouteConfig::getCgiMap() const {
	return m_cgi_map;
}



bool RouteConfig::isCgiScript(const std::string& file) const {
	std::size_t pos = 0;
	std::size_t file_length = file.length();
	std::size_t extention_length;

	for (std::set<std::string>::const_iterator it = m_cgi_map.begin();
			it != m_cgi_map.end(); ++it) {
		extention_length = it->length();
		if (file_length >= extention_length) {
			pos = file_length - extention_length;
			if (file.compare(pos,	extention_length, *it) == 0) {
				return (true);
			}
		}
	}	
	return (false);
}

bool RouteConfig::isCgiEnable(void) const {
	return (!m_cgi_map.empty());
}


bool RouteConfig::hasNoConfig() const {
	if (!getRoot().empty()) return (false);
	if (!getUploadDir().empty()) return (false);
	if (!getAccessLog().empty()) return (false);
	if (!getIndexes().empty()) return (false);
	if (!getAllowedMethods().empty()) return (false);
	if (!getRoot().empty()) return (false);
	if (!getCgiMap().empty()) return (false);
	if (hasMaxBodySize()) return (false);
	if (doesRedirect()) return (false);
	if (isAutoindex()) return (false);

	return (true);
}

RouteConfig::~RouteConfig() {

}
