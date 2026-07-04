#include "webserver.hpp"

Server::Server() : RouteConfig(), m_route_tree("/") {
	init();
}

void Server::parseServerName(ContIter &begin) {
	while (begin->is(WORD)) {
		m_hosts.push_back(begin->value);
		begin++;
	}
}

void Server::parseIPort(ContIter &begin) {
	std::cout << "parseIPort\n";
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
	std::cout << "I insert iport " << iport << "\n";
	m_addr.push_back(iport);
	// m_ordered_addr.insert(iport);
	++begin;
}


Server::HandlerFunc Server::getDirectiveHandler(const std::string dir_name) {
	if (s_handlers.find(dir_name) == s_handlers.end()) {
		return (NULL);
	}
	return (s_handlers[dir_name]);
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
		s_handlers["autoindex"] = &Server::parseAutoIndex;
	}
}


Server::MapHandler Server::s_handlers;


Server::~Server() {}

Server::IPort::IPort() {
	m_addr = NULL;
}

Server::IPort::IPort(const Server::IPort& other) :
	m_size(other.m_size),
	m_family(other.m_family),
	create(other.create),
	clean(other.clean){
		m_addr = create();
		// std::cout << "copy constructor: " << m_addr << "\n";
		*m_addr = *other.m_addr;
	}


Server::IPort& Server::IPort::operator=(const Server::IPort& other) {
	if (m_family != other.m_family)
	{
		clean(m_addr);
		m_family = other.m_family;
		m_addr = other.create();
	}
	if (!other.m_addr)
	{
		std::cerr << "what is goiing on\n";
	}
	// std::cout << "copy assignment operator: " << m_addr << "\n";
	*m_addr = *other.m_addr;
	return (*this);
}


Server::IPort::IPort(int family, std::size_t size, 
		sockaddr* (*create)(void), void (*clean)(sockaddr*)) : 
	m_size(size), 
	m_family(family), 
	m_addr(NULL),
	create(create),
	clean(clean){

	}

	addrinfo Server::IPort::getAddrHints() const {
		struct addrinfo hints;

		std::memset(&hints, 0, sizeof(hints));

		hints.ai_family = m_family;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = 0;
		hints.ai_flags = 0; 
		hints.ai_canonname = NULL;
		hints.ai_addr = NULL;
		hints.ai_next = NULL;

		return (hints);
	}

const sockaddr* Server::IPort::get() const {
	return (m_addr);
}

std::string Server::IPort::getFamilyStr(const int family) {
	switch (family) {
		case AF_INET:
			return "AF_INET";
		case AF_INET6:
			return "AF_INET6";
		default:
			return "Not Known";
	}
}

void Server::IPort::print() const {
	std::cout << "hey";
	if (m_family == AF_INET) {
		std::cout << *reinterpret_cast<sockaddr_in*>(	m_addr);
	}
	else if (m_family == AF_INET6) {
		std::cout << *reinterpret_cast<sockaddr_in6*>(m_addr);
	}
}


bool Server::IPort::operator==(const Server::IPort& other) const {
	if (m_family != other.m_family)
		return (false);
	// std::cout << "hello" << *reinterpret_cast<IPortV4*>(other.m_addr) << "\n";
	// std::cout << "hey" << *reinterpret_cast<IPortV4*>(m_addr) << "\n";
	std::cout << m_addr << " " << other.m_addr << "\n";
	return (std::memcmp(m_addr, other.m_addr, m_size) == 0);
}


Server::IPort::~IPort(){
	delete m_addr;
}

int Server::IPort::getFamily() const {
	return (m_family);
}


int Server::IPort::getSize() const {
	return (m_size);
}


std::ostream& operator<<(std::ostream& out, const sockaddr_in& addr) {
	char buffer[INET6_ADDRSTRLEN] =  {0};
	in_port_t port;
	const char *success = inet_ntop(AF_INET, &addr.sin_addr, buffer, INET_ADDRSTRLEN);
	if (!success)
	{

	}
	port = ntohs(addr.sin_port);
	return (out << buffer << ":" << port);
}


std::ostream& operator<<(std::ostream& out, const sockaddr_in6& addr) {
	char buffer[INET6_ADDRSTRLEN] =  {0};
	in_port_t port;
	const char *success = inet_ntop(AF_INET, &addr.sin6_addr, buffer, INET6_ADDRSTRLEN);
	if (!success)
	{

	}
	port = ntohs(addr.sin6_port);
	return (out << buffer << ":" << port << " family = ");
}


std::ostream& operator<<(std::ostream& out, const Server::IPort& iport) {
	if (iport.getFamily() == AF_INET) {
		out << *reinterpret_cast<const sockaddr_in*>(iport.get());
	}
	else if (iport.getFamily() == AF_INET6) {
		out << *reinterpret_cast<const sockaddr_in6*>(iport.get());
	}
	return (out);
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
	return tokens;
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
		std::vector<std::string> tokens = tokenizeRoutePath(m_locations[i].m_location);

		currentNode = &m_route_tree; 

		for (size_t j = 0; j < tokens.size(); ++j) {
			const std::string& token = tokens[j];

			if (currentNode->children.find(token) == currentNode->children.end()) {
				currentNode->children[token] = new RouteNode(token);
			}
			currentNode = currentNode->children[token];
		}
		currentNode->config = &m_locations[i]; 
	}
}
