#include "webserver.hpp"
#include <cstring>

Server::IPort::IPort() {
	std::memset(&m_addr, 0, sizeof(m_addr));
}

Server::IPort::IPort(const Server::IPort& other) :
	m_size(other.m_size),
	m_family(other.m_family) {
		std::memset(&m_addr, 0, sizeof(m_addr));
		m_addr = other.m_addr;
		m_ip_str = other.m_ip_str;
		m_port_str = other.m_port_str;
	}


Server::IPort::IPort(int family, std::size_t size) : 
	m_size(size), 
	m_family(family) {
		std::memset(&m_addr, 0, sizeof(m_addr));
	}


Server::IPort::IPort(const sockaddr_storage& addr) {
	std::memset(&m_addr, 0, sizeof(m_addr));
	m_family = addr.ss_family;
	if (m_family == AF_INET) {
		*this = IPortV4(*reinterpret_cast<const sockaddr_in*>(&addr));
	}
	else {
		*this = IPortV6(*reinterpret_cast<const sockaddr_in6*>(&addr));
	}
}

Server::IPort& Server::IPort::operator=(const Server::IPort& other) {
	if (m_family != other.m_family) {
		m_family = other.m_family;
		m_size = other.m_size;
	}

	m_addr = other.m_addr;

	m_ip_str = other.m_ip_str;
	m_ip_str = other.m_port_str;

	return (*this);
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
	return (reinterpret_cast<const sockaddr*>(&m_addr));
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
std::string Server::IPort::getPortStr() const{
	return (m_port_str);
}

std::string Server::IPort::getIpStr() const{
	return (m_ip_str);
}

void Server::IPort::print() const {
	std::cout << *this << "\n";
}

bool Server::IPort::operator==(const Server::IPort& other) const {
	if (m_family != other.m_family)
		return (false);
	return (std::memcmp(&m_addr, &other.m_addr, m_size) == 0);
}

Server::IPort::~IPort(){}

int Server::IPort::getFamily() const {
	return (m_family);
}

int Server::IPort::getSize() const {
	return (m_size);
}


Server::~Server() {}

std::ostream& operator<<(std::ostream& out, const Server::IPort& iport) {
	return (out << "ip = [" << iport.getIpStr() <<  "]" << " port = [" << iport.getPortStr() << "]");
}

