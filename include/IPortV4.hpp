#ifndef _IPORTV4_HPP_
#define _IPORTV4_HPP_

#include "webserver.hpp"
#include "Server.hpp"

struct Server::IPortV4 : public Server::IPort, public Server::ParseIPortInterface{
	IPortV4();
	virtual void setIp(const std::string& ip);
	virtual void setPort(const std::string& port);
	// virtual void print() const;
	virtual bool operator==(const IPortV4& other) const;
	bool isStrictIp(const std::string& ip);
	private:
	sockaddr_in* m_addr;
};


#endif
