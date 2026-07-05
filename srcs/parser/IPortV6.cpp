#include "webserver.hpp"

Server::IPortV6::IPortV6() : IPort(AF_INET6, sizeof(sockaddr_in6), &IPortV4::create, &IPortV4::clean) {
	m_addr = new sockaddr_in6;
	IPort::m_addr = reinterpret_cast<sockaddr *>(m_addr);
	std::memset(m_addr, 0, sizeof(sockaddr_in6));
	m_addr->sin6_family = m_family;
	m_addr->sin6_port = htons(DEFAULT_PORT);
}

bool Server::IPortV6::operator==(const Server::IPortV6& other) const {
	return (std::memcmp(&m_addr, &other.m_addr, sizeof(sockaddr_in6)));
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
			sockaddr_in6* hey = reinterpret_cast<sockaddr_in6 *>(tmp->ai_addr);
			*m_addr = *hey;
			break;
		}
		freeaddrinfo(res);
	}
}

void Server::IPortV6::clean(sockaddr *addr) {
	delete (reinterpret_cast<sockaddr_in6*>(addr));
}

void Server::IPortV6::setPort(const std::string& port_str) {
	in_port_t port;
	std::stringstream ss(port_str);
	ss >> port;
	m_addr->sin6_port = htons(port);
}

bool Server::IPortV6::isStrictIp(const std::string& ip) {
	int success = inet_pton(m_family, ip.c_str(), &m_addr->sin6_addr);
	return (success);
}


sockaddr* Server::IPortV6::create() {
	return (reinterpret_cast<sockaddr*>(new sockaddr_in6));
}
