#include "webserver.hpp"
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>

Server::IPortV6::IPortV6() : IPort(AF_INET6, sizeof(sockType)) {
	m_addr = reinterpret_cast<sockType *>(&(IPort::m_addr));
	m_addr->sin6_family = m_family;
	// TODO: 
	// here i need to add ip mybe 0
	// m_addr->sin6_addr
	m_addr->sin6_port = htons(DEFAULT_PORT);
	setIpString();
	setPortString();
}

Server::IPortV6::IPortV6(const sockType& addr) : IPort(AF_INET6, sizeof(sockType)) {
	m_addr = reinterpret_cast<sockType *>(&(IPort::m_addr));
	*m_addr = addr;
	setIpString();
	setPortString();
}

Server::IPortV6::IPortV6(const std::string& ip, const std::string& port) : IPort(AF_INET6, sizeof(sockType)) {
	m_addr = reinterpret_cast<sockType *>(&(IPort::m_addr));
	setIp(ip);
	setPort(port);

	setIpString();
	setPortString();
}

void Server::IPortV6::setIpString() {
	char buffer[INET6_ADDRSTRLEN] =  {0};
	const char *success = inet_ntop(m_family, &m_addr->sin6_addr, buffer, INET_ADDRSTRLEN);
	if (!success) {	}
	m_ip_str = buffer;
}


void Server::IPortV6::setPortString() {
	in_port_t port = ntohs(m_addr->sin6_port);
	std::stringstream ss;
	ss << port;
	m_port_str = ss.str();
}

bool Server::IPortV6::operator==(const Server::IPortV6& other) const {
	return (std::memcmp(&m_addr, &other.m_addr, m_size) == 0);
}

void Server::IPortV6::setIp(const std::string& ip) {
	addrinfo hints = getAddrHints();
	addrinfo *res;
	addrinfo *tmp;

	if (!isStrictIp(ip))	{
		int fail = getaddrinfo(ip.c_str(), NULL, &hints, &res);
		if (fail) {
			throw (std::runtime_error("error getaddrinfo"));
		}
		tmp = res;
		for (; tmp != NULL; tmp = tmp->ai_next) {
			sockType* addr = reinterpret_cast<sockType *>(tmp->ai_addr);
			*m_addr = *addr;
			break;
		}
		freeaddrinfo(res);
	}
	setIpString();
}

void Server::IPortV6::setPort(const std::string& port_str) {
	in_port_t port;
	std::stringstream ss(port_str);
	m_port_str = ss.str();
	if (!(ss >> port)) {
		throw (std::runtime_error("malformed port ipv6"));
	}
	m_addr->sin6_port = htons(port);
}

bool Server::IPortV6::isStrictIp(const std::string& ip) {
	int success = inet_pton(m_family, ip.c_str(), &m_addr->sin6_addr);
	return (success);
}

Server::IPortV6::~IPortV6() {}
