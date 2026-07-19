#ifndef _IPORT_H
#define _IPORT_H

#include "webserver.hpp"

struct Server::IPort {

	protected:
		IPort(int family, std::size_t size);

	public:
		// INFO: pass a sockaddr_storage object by ref and the constructor
		// determine is it v4 or v6 and assign m_addr to addr
		IPort(const sockaddr_storage& addr);
		IPort(const IPort& other);
		IPort();

		IPort& operator=(const Server::IPort& other);

		virtual bool operator==(const IPort& other) const;

		virtual void print() const;

		// INFO: return sockaddr pointer
		const sockaddr	*get() const;


		// GETTERS
		int getSize() const;
		std::string getPort();
		int getFamily() const;


		std::string getIpStr() const;
		std::string getPortStr() const;

		std::string getAddress();
		static std::string getFamilyStr(const int family);

		virtual ~IPort();

	protected:
		addrinfo getAddrHints() const;
		std::size_t m_size;
		int m_family;
		sockaddr_storage m_addr;
		std::string m_ip_str;
		std::string m_port_str;

};


#endif
