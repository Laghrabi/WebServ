#ifndef _LOCATION_H
#define _LOCATION_H

#include "webserver.hpp"

class ParseConfig;

class Location : public RouteConfig {
	private:
		std::string m_alias;
	public:
		typedef void (Location::*HandlerFunc)(ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler ;

		Location();
		static Location::HandlerFunc getDirectiveHandler(const std::string dir_name);
		static void init(void);
		bool hasSamePath(const Location& other);
		const std::string& getPath(void) const;
		void setPath(const std::string& path);
		void parseAlias(ContIter& begin);
		const std::string& getAlias(void) const;
		void copyServerRouteConfig(const RouteConfig& route_conf);
		template <typename T> void copyDirectiveInfo(T& var, const T& new_val);
		~Location();

	private:
		static MapHandler s_handlers;
		std::string m_path;
};

#endif
