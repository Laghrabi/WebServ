#include "webserver.hpp"
#include <netinet/in.h>

Server::IPortV4::IPortV4() : IPort(AF_INET, sizeof(sockaddr_in), &IPortV4::create, &IPortV4::clean) {
	m_addr = new sockaddr_in;
	IPort::m_addr = reinterpret_cast<sockaddr *>(m_addr);
	std::memset(m_addr->sin_zero, 0, sizeof(m_addr->sin_zero));
	m_addr->sin_family = m_family;
	m_addr->sin_port = htons(DEFAULT_PORT);
	m_addr->sin_addr.s_addr = htonl(DEFAULT_ADDR);
}

bool Server::IPortV4::isStrictIp(const std::string& ip) {
	int success = inet_pton(m_family, ip.c_str(), &m_addr->sin_addr);
	return (success);
}

bool Server::IPortV4::operator==(const Server::IPortV4& other) const {
	return (std::memcmp(&m_addr, &other.m_addr, sizeof(sockaddr_in)));
}

void Server::IPortV4::setIp(const std::string& ip) {
	addrinfo hints = getAddrHints();
	addrinfo *res;
	addrinfo *tmp;

	std::cout << "new setIp \n";
	if (!isStrictIp(ip))	{
		std::cout << "not strict " << ip << "\n";
		int fail = getaddrinfo(ip.c_str(), NULL, &hints, &res);
		if (fail) {
			throw (std::runtime_error("error getaddrinfo"));
		}
		tmp = res;
		for (; tmp != NULL; tmp = tmp->ai_next) {
			sockaddr_in* hey = reinterpret_cast<sockaddr_in *>(tmp->ai_addr);
			*m_addr = *hey;
			break;
		}
		freeaddrinfo(res);
	}
}

void Server::IPortV4::setPort(const std::string& port_str) {
	in_port_t port;
	std::stringstream ss(port_str);
	ss >> port;
	m_addr->sin_port = htons(port);
}

std::string Server::IPortV4::info(const sockaddr_in& addr) {
	char buffer[INET_ADDRSTRLEN] =  {0};
	in_port_t port;
	std::stringstream port_str;
	std::string res;

	const char *success = inet_ntop(AF_INET, &addr.sin_addr, buffer, INET_ADDRSTRLEN);
	if (!success)
	{
		// if not success (idont know exactely how to handle that but i will figure it out)
	}
	port = ntohs(addr.sin_port);
	port_str << port;
	res = std::string(buffer) + ":" + port_str.str() + " family = " + IPort::getFamilyStr(addr.sin_family);
	return (res);
}

void Server::IPortV4::clean(sockaddr *addr) {
	delete (reinterpret_cast<sockaddr_in*>(addr));
}

sockaddr* Server::IPortV4::create() {
	return (reinterpret_cast<sockaddr*>(new sockaddr_in));
}
