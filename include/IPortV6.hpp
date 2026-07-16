#ifndef _IPORTV6_HPP_
#define _IPORTV6_HPP_

#include "webserver.hpp"

struct Server::IPortV6 : public Server::IPort, public Server::ParseIPortInterface{

	protected:

	virtual bool isStrictIp(const std::string& ip);
	virtual void setPortString();
	virtual void setIpString();


	public:
	typedef sockaddr_in6 sockType;

	IPortV6();
	IPortV6(const sockType& addr);
	IPortV6(const std::string& ip, const std::string& port);
	virtual void setIp(const std::string& ip);
	virtual void setPort(const std::string& port);


	virtual bool operator==(const IPortV6& other) const;
	~IPortV6();

	private:
	sockType* m_addr;

};


#endif
