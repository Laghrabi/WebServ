#include "RouteConfig.hpp"
#include "webserver.hpp"
#include <cstring>
#include <map>
#include <netinet/in.h>
#include <stdexcept>
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
	IPortV4 iport;

	std::string ip;
	std::string port;

	if ((pos = iport_str.find(":")) != std::string::npos) {
		ip = iport_str.substr(0, pos);
		port = iport_str.substr(pos + 1);

		iport.setIp(ip);
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

	(void)other;
	(void)server_name;
	// bool same_host = false;
	// bool same_iport = false;
	//
	// for (std::set<std::string>::const_iterator it = other.m_ordered_hosts.begin();it != other.m_ordered_hosts.end() && !same_host; ++it) {
	// 	if (m_ordered_hosts.find(*it) != m_ordered_hosts.end())	
	// 	{
	// 		same_host = true;
	// 		server_name = *it;
	// 	}
	// }
	// if (!same_host)
	// 	return (false);
	//
	// for (std::set<Server::IPort>::const_iterator it = other.m_ordered_addr.begin();
	// 		it != other.m_ordered_addr.end() && !same_iport; ++it) {
	// 	if (m_ordered_addr.find(*it) != m_ordered_addr.end())	
	// 		same_iport = true;
	// }
	// if (!same_iport)
	// 	return (false);
	//
	// return (true);

	return (false);
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

// Server::IPort::IPort(){}
//
// Server::IPort::IPort(in_addr_t addr, in_port_t port) :
// 	m_addr_ip(addr),
// 	m_port(port) {}
//
//
// 	bool Server::IPort::operator==(const IPort& other) const {
// 		return (m_addr_ip == other.m_addr_ip && m_port == other.m_port);
// 	}
//
// void Server::IPort::setIp(const std::string& ip) throw(std::exception) {
// 	struct in_addr addr;
// 	int sucess;
//
// 	sucess = inet_pton(AF_INET, ip.c_str(), &addr);
// 	if (sucess == 1) {
// 		m_addr_ip = addr.s_addr;	
// 	}
// 	else {
// 		if (sucess == 0)	
// 		{
// 			throw (std::exception());
// 		}
// 		else {
// 			perror("error inet_pton");
// 			throw (std::exception());
// 		}
// 	}
// }
//
// void Server::IPort::setPort(const std::string& port) throw(std::exception) {
// 	std::stringstream ss(port);
// 	ss >> m_port;
// 	if (ss.fail())
// 		throw (std::exception());
// }
//
// void Server::IPort::setIp(in_addr_t ip) {
// 	m_addr_ip = ip;
// }
// // . This is the perfect reason to use inheritance over composition.
// const in_port_t& Server::IPort::getPort() const {
// 	return (m_port);
// }
//
// const in_addr_t& Server::IPort::getAddr() const {
// 	return (m_addr_ip);
// }


// bool Server::IPort::operator<(const IPort& other) const {
// 	if (m_addr_ip < other.m_addr_ip)
// 		return (true);
// 	if (m_port < other.m_port)
// 		return (true);
// 	return (false);
// }


Server::MapHandler Server::s_handlers;


Server::~Server() {}

Server::IPort::IPort() {

}
Server::IPort::IPort(const Server::IPort& other) : m_famlily(other.m_famlily),
	m_size(other.m_size){
		if (m_famlily == AF_INET) {
			m_addr = reinterpret_cast<sockaddr*>(new sockaddr_in);
		}
		if (m_famlily == AF_INET6) {
			m_addr = reinterpret_cast<sockaddr*>(new sockaddr_in6);
		}
		m_addr = other.m_addr;
	}
Server::IPort::IPort(int family, std::size_t size) : m_famlily(family), m_size(size) {}

addrinfo Server::IPort::getAddrHints() const {
	struct addrinfo hints;

	std::memset(&hints, 0, sizeof(hints));

	hints.ai_family = m_famlily;
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

void Server::IPort::print() const {
	std::cout << "hey";
	if (m_famlily == AF_INET) {
		std::cout << *reinterpret_cast<sockaddr_in*>(	m_addr);
	}
	else if (m_famlily == AF_INET6) {
		std::cout << *reinterpret_cast<sockaddr_in6*>(m_addr);
	}
}


bool Server::IPort::operator==(const Server::IPort& other) const {
	if (m_famlily != other.m_famlily)
		return (false);
	return (std::memcmp(m_addr, other.m_addr, m_size) == 0);
}


bool Server::IPort::operator<(const Server::IPort& other) const {
	(void)other;
	return (true);
}


Server::IPort::~IPort(){}


bool Server::IPortV4::isStrictIp(const std::string& ip) {
	int success = inet_pton(m_famlily, ip.c_str(), &m_addr->sin_addr);
	return (success);
}

Server::IPortV4::IPortV4() : IPort(AF_INET, sizeof(sockaddr_in)) {
	IPort::m_addr = reinterpret_cast<sockaddr *>(new sockaddr_in());
	m_addr = reinterpret_cast<sockaddr_in *>(IPort::m_addr);
}


bool Server::IPortV4::operator==(const Server::IPort& other) const {
	if (!(IPort::operator==(other))) {
		return (false);
	}
	const IPortV4 &otherV4 = static_cast<const Server::IPortV4&>(other);
	return (std::memcmp(&m_addr, &otherV4.m_addr, sizeof(sockaddr_in)));
}


void Server::IPortV4::setIp(const std::string& ip) {
	addrinfo hints = getAddrHints();
	addrinfo *ptr;
	addrinfo *tmp;

	std::cout << "new setIp \n";
	if (!isStrictIp(ip))	{
		std::cout << "not strict " << ip << "\n";
		int fail = getaddrinfo(ip.c_str(), NULL, &hints, &ptr);
		if (fail) {
			throw (std::runtime_error("error getaddrinfo"));
		}
		tmp = ptr;
		for (; tmp != NULL; tmp = tmp->ai_next) {
			sockaddr_in* hey = reinterpret_cast<sockaddr_in *>(tmp->ai_addr);
			*m_addr = *hey;
			break;
		}
	}
	else {
		std::cout << "it is strict ip " << ip << "\n";
	}
}

void Server::IPortV4::setPort(const std::string& port_str) {
	in_port_t port;
	std::stringstream ss(port_str);
	ss >> port;
	m_addr->sin_port = htons(port);
}

// void Server::IPortV4::print() const {
// 	char buffer[INET_ADDRSTRLEN] =  {0};
// 	const char *addr_str = inet_ntop(m_famlily, &m_addr.sin_addr, buffer, INET_ADDRSTRLEN);
//
// 	std::cout << "Ip = " << addr_str << "port = "  << ntohs(m_addr.sin_port) << "\n";
// }

Server::IPortV6::IPortV6() : IPort(AF_INET6, sizeof(sockaddr_in6)) {
	IPort::m_addr = reinterpret_cast<sockaddr *>(new sockaddr_in6());
	m_addr = reinterpret_cast<sockaddr_in6 *>(IPort::m_addr);
}


bool Server::IPortV6::operator==(const Server::IPort& other) const {
	if (!(IPort::operator==(other))) {
		return (false);
	}
	const IPortV6 &otherV6 = static_cast<const Server::IPortV6&>(other);

	return (std::memcmp(&m_addr, &otherV6.m_addr, sizeof(sockaddr_in6)));
}


void Server::IPortV6::setIp(const std::string& ip) {
	addrinfo hints = getAddrHints();
	addrinfo *ptr;
	addrinfo *tmp;

	if (!isStrictIp(ip))	{
	}
	int fail = getaddrinfo(ip.c_str(), NULL, &hints, &ptr);
	if (fail) {
		throw (std::runtime_error("error getaddrinfo"));
	}
	tmp = ptr;
	for (; tmp != NULL; tmp = tmp->ai_next) {
		sockaddr_in6* hey = reinterpret_cast<sockaddr_in6 *>(tmp->ai_addr);
		*m_addr = *hey;
		break;
	}
}

void Server::IPortV6::setPort(const std::string& port_str) {
	in_port_t port;
	std::stringstream ss(port_str);
	ss >> port;
	m_addr->sin6_port = htons(port);
}


bool Server::IPortV6::isStrictIp(const std::string& ip) {
	int fail = inet_pton(m_famlily, ip.c_str(), &m_addr->sin6_addr);
	return (!fail);
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
	return (out << buffer << ":" << port);
}


std::ostream& operator<<(std::ostream& out, const Server::IPort& iport) {
	if (iport.m_famlily == AF_INET) {
		out << *reinterpret_cast<const sockaddr_in*>(iport.get());
	}
	else if (iport.m_famlily == AF_INET6) {
		out << *reinterpret_cast<const sockaddr_in6*>(iport.get());
	}
	return (out);
}


// void Server::IPortV6::print() const {
// 	char buffer[INET6_ADDRSTRLEN] =  {0};
// 	const char *addr_str = inet_ntop(m_famlily, &m_addr.sin6_addr, buffer, INET6_ADDRSTRLEN);
//
// 	std::cout << "Ip = " << addr_str << "port = "  << ntohs(m_addr.sin6_port) << "\n";
// }
