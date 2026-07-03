#include "IPortV4.hpp"

Server::IPortV4::IPortV4() : IPort(AF_INET, sizeof(sockaddr_in)) {
	IPort::m_addr = reinterpret_cast<sockaddr *>(new sockaddr_in());
	m_addr = reinterpret_cast<sockaddr_in *>(IPort::m_addr);
}

bool Server::IPortV4::isStrictIp(const std::string& ip) {
	int success = inet_pton(m_famlily, ip.c_str(), &m_addr->sin_addr);
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
