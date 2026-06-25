#include "RouteConfig.hpp"
#include "webserver.hpp"
#include <cstring>
#include <map>
#include <sys/socket.h>
#include "Server.hpp"
#include "ParseConfig.hpp"

Server::Server() : RouteConfig() {
	init();
}

void Server::parseServerName(ContIter &begin) {
	while (begin->is(WORD)) {
		m_hosts.push_back(begin->value);
		m_ordered_hosts.insert(begin->value);
		begin++;
	}
}

void Server::parseIPort(ContIter &begin) {
	std::string iport_str = begin->value;
	size_t pos;
	IPort iport(0, 80);

	std::string ip;
	std::string port;

	if ((pos = iport_str.find(":")) != std::string::npos) {
		ip = iport_str.substr(0, pos);
		port = iport_str.substr(pos + 1);

		iport.setPort(ip);
		iport.setPort(port);

		std::cout << ip << "\n";
		std::cout << "\n";
		std::cout << port << "\n";

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
	struct sockaddr hey;
	m_addr.push_back(iport);
	m_ordered_addr.insert(iport);
	++begin;
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_addr = iport.getAddr();
	int success = getaddrinfo("google.com", NULL, );
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

	for (std::set<std::string>::const_iterator it = other.m_ordered_hosts.begin();it != other.m_ordered_hosts.end() && !same_host; ++it) {
		if (m_ordered_hosts.find(*it) != m_ordered_hosts.end())	
		{
			same_host = true;
			server_name = *it;
		}
	}
	if (!same_host)
		return (false);

	for (std::set<Server::IPort>::const_iterator it = other.m_ordered_addr.begin();
			it != other.m_ordered_addr.end() && !same_iport; ++it) {
		if (m_ordered_addr.find(*it) != m_ordered_addr.end())	
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

Server::IPort::IPort(){}

Server::IPort::IPort(in_addr_t addr, in_port_t port) :
	m_addr_ip(addr),
	m_port(port) {}


	bool Server::IPort::operator==(const IPort& other) const {
		return (m_addr_ip == other.m_addr_ip && m_port == other.m_port);
	}

void Server::IPort::setIp(const std::string& ip) throw(std::exception) {
	struct in_addr addr;
	int sucess;

	sucess = inet_pton(AF_INET, ip.c_str(), &addr);
	if (sucess == 1) {
		m_addr_ip = addr.s_addr;	
	}
	else {
		if (sucess == 0)	
		{
			throw (std::exception());
		}
		else {
			perror("error inet_pton");
			throw (std::exception());
		}
	}
}

void Server::IPort::setPort(const std::string& port) throw(std::exception) {
	std::stringstream ss(port);
	ss >> m_port;
	if (ss.fail())
		throw (std::exception());
}

void Server::IPort::setIp(in_addr_t ip) {
	m_addr_ip = ip;
}
// . This is the perfect reason to use inheritance over composition.
const in_port_t& Server::IPort::getPort() const {
	return (m_port);
}

const in_addr_t& Server::IPort::getAddr() const {
	return (m_addr_ip);
}

void Server::make_pair(std::multimap<Server::IPort, Server>& iport_server_map) const{
	for (std::vector<IPort>::const_iterator it = m_addr.begin(); it != m_addr.end(); ++it) {
		iport_server_map.insert(std::make_pair(*it, *this));
	}
}

bool Server::IPort::operator<(const IPort& other) const {
	if (m_addr_ip < other.m_addr_ip)
		return (true);
	if (m_port < other.m_port)
		return (true);
	return (false);
}

std::ostream& operator<<(std::ostream& out, const Server::IPort& iport) {
	out << "port = " << iport.getPort() << " ip = " << iport.getAddr() << "\n";
	return (out);
}

Server::MapHandler Server::s_handlers;

Server::~Server() {}

const sockaddr* Server::IPortV4::get() const {
	return (reinterpret_cast<const sockaddr*>(&m_addr));
}
void Server::IPortV4::setIp(const std::string& ip) {

}
void Server::IPortV4::setPort(const std::string& port) {

}
void Server::IPortV4::print() const {

}


const sockaddr* Server::IPortV6::get() const {
	return (reinterpret_cast<const sockaddr*>(&m_addr));
}
void Server::IPortV6::setIp(const std::string& ip) {
}
void Server::IPortV6::setPort(const std::string& port) {
}
void Server::IPortV6::print() const {

}
bool Server::IPortV6::operator==(const Server::IPortInterface& other) const {
	return (m_famlily == other.getFamily());
}


