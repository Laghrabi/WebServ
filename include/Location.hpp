#ifndef _LOCATION_H
#define _LOCATION_H

#include "webserver.hpp"

class ParseConfig;

class Location : public RouteConfig {
	public:
		typedef void (Location::*HandlerFunc)(ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler ;

		Location();
		static void init(void);
		bool hasSamePath(const Location& other);
		const std::string& getPath(void) const;
		~Location();

	private:
		static MapHandler s_handlers;
		std::string m_path;
};

#endif
