#include "webserver.hpp"
#include "Server.hpp"
#include <exception>
#include <arpa/inet.h>
#include <string.h>


Server::IPort Server::ParseServer::parseIPort(std::string iport_str) {
	size_t pos = iport_str.find(":");
	IPort iport;

	if ((pos = iport_str.find(":")) != std::string::npos) {
		std::string ip(iport_str, 0, pos);
		std::string port(iport_str, pos + 1);

		std::cout << ip;
		std::cout << "\n";
		std::cout << port;

	}
	else if (iport_str.find(".") != std::string::npos) {
		;
	}
	else {
		;
	}
	return (Server::IPort(0, 80));
}

void Server::IPort::setIp(const std::string& ip) throw(std::exception) {
	struct in_addr addr;
	int sucess;

	sucess = inet_pton(AF_INET, ip, &addr);
	if (sucess == 1){
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
		if (ss.fail() || ss.tellg() != port.length()	)
			throw (std::exception());
}

void Server::IPort::setPort(in_port_t port) {
	m_port = port;
}

void Server::IPort::setIp(in_addr_t ip) {
	m_addr_ip = ip;
}
