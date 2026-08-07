#include "webserver.hpp"

Server::Server() : RouteConfig(), m_route_tree("/") {
	init();
}

Server::Server(const Server& other)
	: RouteConfig(other),
	m_locations(other.m_locations),
	m_route_tree(other.m_route_tree),
	m_addr(other.m_addr),
	m_hosts(other.m_hosts)
{
}

Server& Server::operator=(const Server& other)
{
	RouteConfig::operator=(other);
	m_locations = other.m_locations;
	m_route_tree = other.m_route_tree;
	m_addr = other.m_addr;
	m_hosts = other.m_hosts;
	return *this;
}

void Server::parseServerName(ContIter &begin) {
	while (begin->is(WORD)) {
		m_hosts.push_back(begin->value);
		begin++;
	}
}


void Server::parseIPort(ContIter &begin) {
	std::string iport_str = begin->value;
	size_t pos;
	IPortV4 iport;

	std::string ip;
	std::string port;

	if ((pos = iport_str.find(":")) != std::string::npos) {
		ip = iport_str.substr(0, pos);
		port = iport_str.substr(pos + 1);

		iport.setIp(ip);
		iport.setPort(port);

	}
	else if (iport_str.find(".") != std::string::npos) {
		iport.setIp(iport_str);
	}
	else {
		iport.setPort(iport_str);
	}
	if (std::find(m_addr.begin(), m_addr.end(), iport) != m_addr.end())
	{
		std::cout << "hey found duplacate";
		throw (ParseConfigType::ConfigExcept("duplcate iport", begin->line));
	}
	else {
		std::cout << (std::find(m_addr.begin(), m_addr.end(), iport) == m_addr.end()) << "\n";
	}
	// std::cout << "I insert iport " << iport << "\n";
	m_addr.push_back(iport);
	// m_ordered_addr.insert(iport);
	++begin;
}


void Server::parseErrorPage(ContIter &begin) {
	int code = 0;
	if (!toNum<int>(code, begin->value)) {
		throw(ParseConfigType::ConfigExcept("not valid code: \"" + begin->value + "\"", begin->line));
	}

	// check if there is duplicate
	// check here if error code is valid or not
	if (code < 300 || code > 599) {
		throw (ParseConfigType::ConfigExcept("error page code must be between 300 and 599", begin->line));
	}
	++begin;
	if (!begin->is(WORD)) {
		throw (ParseConfigType::ConfigExcept("expection error page", begin->line));
	}

	std::map<int, std::string>::const_iterator it = m_error_pages.find(code);
	if (it == m_error_pages.end()) {
		m_error_pages[code] = begin->value;
	}
	else {
		throw (ParseConfigType::ConfigExcept("duplicate error page in the same server is not allowed", begin->line));
	}
	++begin;
}


Server::HandlerFunc Server::getDirectiveHandler(const std::string dir_name) {
	init();
	if (s_handlers.find(dir_name) == s_handlers.end()) {
		return (NULL);
	}
	return (s_handlers[dir_name]);
}


bool Server::hasServerName(const std::string& name) const {
	return (elemExist(m_hosts, name));
}

const std::vector<std::string>& Server::getServerNames(void) const{
	return (m_hosts);
}


bool Server::conflictsWith(const Server& other, std::string& server_name) const{
	bool same_host = false;
	bool same_iport = false;

	for (std::vector<std::string>::const_iterator it = other.m_hosts.begin();it != other.m_hosts.end() && !same_host; ++it) {
		if (elemExist(m_hosts, *it))	
		{
			same_host = true;
			server_name = *it;
		}
	}
	if (!same_host)
		return (false);

	for (std::vector<Server::IPort>::const_iterator it = other.m_addr.begin();
			it != other.m_addr.end() && !same_iport; ++it) {
		if (elemExist(m_addr, *it))	
			same_iport = true;
	}
	if (!same_iport)
		return (false);

	return (true);
}

void Server::init() {
	RouteConfig::init();
	if (s_handlers.empty()) {
		for (RouteConfig::MapHandler::const_iterator it = RouteConfig::s_handlers.begin(); it != RouteConfig::s_handlers.end(); ++it) {
			s_handlers[it->first] = it->second;
		}
		s_handlers["access_log"] = &Server::parseAccessLog;
		s_handlers["server_name"] = &Server::parseServerName;
		s_handlers["listen"] = &Server::parseIPort;
		s_handlers["error_page"] = &Server::parseErrorPage;
	}
}


Server::MapHandler Server::s_handlers;


const std::vector<Server::IPort>& Server::getAddrs(void) const{
	return (m_addr);
}


// void Server::IPortV6::print() const {
// 	char buffer[INET6_ADDRSTRLEN] =  {0};
// 	const char *addr_str = inet_ntop(m_family, &m_addr.sin6_addr, buffer, INET6_ADDRSTRLEN);
//
// 	std::cout << "Ip = " << addr_str << "port = "  << ntohs(m_addr.sin6_port) << "\n";
// }

/**
 * @brief Tokenizes a raw URI path into individual segments.
 * * Splits a path string by the '/' delimiter. Consecutive slashes are ignored, 
 * ensuring clean tokens (e.g., "/api//users/" becomes ["api", "users"]).
 * * @param path The raw URI path string to tokenize.
 * @return A vector of path segments.
 */
std::vector<std::string> tokenizeRoutePath(const std::string& path) {
	std::vector<std::string> tokens;
	std::string current = "";

	if (path.length() == 1 && path[0] == '/') {
		tokens.push_back(path);
		return (tokens);
	}
	else {
		tokens.push_back("/");
	}

	for (size_t i = 0; i < path.length(); ++i) {
		if (path[i] == '/') {
			if (!current.empty()) {
				tokens.push_back(current);
				current = "";
			}
		} else {
			current += path[i];
		}
	}
	if (!current.empty()) {
		tokens.push_back(current);
	}
	return (tokens);
}

/**
 * @brief Constructs the hierarchical routing tree (Trie) for the server.
 * * Iterates through all configured location blocks, tokenizing their paths 
 * and inserting them into the routing tree. Each path segment becomes a node, 
 * with the terminal node of each location path storing a pointer to its 
 * corresponding RouteConfig.
 */
void Server::buildRouteTree() {
	RouteNode* currentNode = &m_route_tree; 

	for (size_t i = 0; i < m_locations.size(); ++i) {
		std::vector<std::string> tokens = tokenizeRoutePath(m_locations[i].getPath());
		currentNode = &m_route_tree; 

		std::cout << " hey " << m_locations[i].getPath() << tokens.size() << "\n";
		for (size_t j = 0; j < tokens.size(); ++j) {
			const std::string& token = tokens[j];
			std::cout << "token = " << j << token << "\n";

			if (currentNode->children.find(token) == currentNode->children.end()) {
				currentNode->children.insert(std::make_pair(token, new RouteNode(token)));
			}
			currentNode = currentNode->children[token];
		}
		m_locations[i].copyServerRouteConfig(*this);
		currentNode->config = new Location;
		*currentNode->config = m_locations[i]; 
	}
}
