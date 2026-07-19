#include "webserver.hpp"
#include <netinet/in.h>

Server::IPortV4::IPortV4() : IPort(AF_INET, sizeof(sockType)) {
	m_addr = reinterpret_cast<sockType *>(&(IPort::m_addr));
	m_addr->sin_family = m_family;
	m_addr->sin_port = htons(DEFAULT_PORT);
	m_addr->sin_addr.s_addr = htonl(DEFAULT_ADDR);
	setIpString();
	setPortString();
}

Server::IPortV4::IPortV4(const sockType& addr) : IPort(AF_INET, sizeof(sockType)) {
	m_addr = reinterpret_cast<sockType *>(&(IPort::m_addr));
	*m_addr = addr;
	setIpString();
	setPortString();
}

Server::IPortV4::IPortV4(const std::string& ip, const std::string& port) {
	m_addr = reinterpret_cast<sockType *>(&(IPort::m_addr));
	setIp(ip);
	setPort(port);

	setIpString();
	setPortString();
}

void Server::IPortV4::setIpString() {
	char buffer[INET_ADDRSTRLEN] =  {0};
	const char *success = inet_ntop(m_family, &m_addr->sin_addr, buffer, INET_ADDRSTRLEN);
	if (!success) {	}
	std::cout << buffer << "hello\n";
	m_ip_str = buffer;
}


void Server::IPortV4::setPortString() {
	in_port_t port = ntohs(m_addr->sin_port);
	std::stringstream ss;
	ss << port;
	m_port_str = ss.str();
}

bool Server::IPortV4::isStrictIp(const std::string& ip) {
	int success = inet_pton(m_family, ip.c_str(), &m_addr->sin_addr);
	return (success);
}


bool Server::IPortV4::operator==(const Server::IPortV4& other) const {
	return (std::memcmp(&m_addr, &other.m_addr, m_size) == 0);
}

void Server::IPortV4::setIp(const std::string& ip) {
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

void Server::IPortV4::setPort(const std::string& port_str) {
	in_port_t port;
	std::stringstream ss(port_str);
	m_port_str = ss.str();
	ss >> port;
	m_addr->sin_port = htons(port);
}

std::string Server::IPortV4::info(const sockType& addr) {
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

Server::IPortV4::~IPortV4() {}
