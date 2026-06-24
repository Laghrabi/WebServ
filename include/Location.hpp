#ifndef _LOCATION_H
#define _LOCATION_H

#include "RouteConfig.hpp"
#include "webserver.hpp"

class ParseConfig;

class Location : public RouteConfig {
	public:
		typedef std::vector<token> Container;
		typedef Container::iterator ContIter;
		typedef void (Location::*HandlerFunc)(ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler ;

		Location();

		static void init(void);
		~Location();


	private:
		static MapHandler s_handlers;
};

#endif
