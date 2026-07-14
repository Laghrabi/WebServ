#ifndef _IPORTV4_HPP_
#define _IPORTV4_HPP_

#include "webserver.hpp"

struct Server::IPortV4 : public Server::IPort, public Server::ParseIPortInterface{

	protected:

	bool isStrictIp(const std::string& ip);
	virtual void setPortString();
	virtual void setIpString();

	public:

	typedef sockaddr_in sockType;

	IPortV4();
	IPortV4(const std::string& ip, const std::string& port);
	IPortV4(const sockaddr_in& addr);
	virtual bool operator==(const IPortV4& other) const;

	virtual void setIp(const std::string& ip);
	virtual void setPort(const std::string& port);

	std::string info(const sockaddr_in& addr);
	~IPortV4();

	private:

	sockType* m_addr;

};


#endif
