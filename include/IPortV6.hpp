#ifndef _IPORTV6_HPP_
#define _IPORTV6_HPP_

#include "webserver.hpp"

struct Server::IPortV6 : public Server::IPort, public Server::ParseIPortInterface{
	IPortV6();
	virtual void setIp(const std::string& ip);
	virtual void setPort(const std::string& port);
	// virtual void print() const;
	virtual bool operator==(const IPortV6& other) const;
	static void clean(sockaddr *addr);
	bool isStrictIp(const std::string& ip);
	static sockaddr* create();
	private:
	sockaddr_in6* m_addr;
};


#endif
