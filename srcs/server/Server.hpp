#ifndef _SERVER_H
#define _SERVER_H

#include <iostream>
#include <string>
#include <netinet/in.h>
#include <list>
#include <set>
#include <forward_list>

typedef std::string dir_t;
// namespace 

class Server {
	private:
		struct IPort {
			in_addr_t m_addr;	
			in_port_t m_port;
		};

		class Location {
			public:
				std::string m_location;
				std::forward_list<std::string> m_indexes;
				std::string m_root;
				std::string m_upload_dir;
		};

		std::forward_list<IPort> m_addr;
		std::set<std::string> m_host;
		std::string m_root;
		std::forward_list<std::string> m_indexes;
		std::string m_upload_dir;

	
		;


	public:
		Server();
};

#endif
