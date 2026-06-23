#ifndef _LOCATION_H
#define _LOCATION_H

#include "webserver.hpp"

class ParseConfig;

class Location {
	public:
		typedef std::vector<token> Container;
		typedef Container::iterator ContIter;

		typedef void (Location::*HandlerFunc)(ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler ;
		typedef ParseConfig ParseConfigType ;

		static MapHandler s_handlers;

		Location();

		static HandlerFunc getDirectiveHandler(const std::string dir_name);
		static void init(void);

		void parseAutoIndex(ContIter &begin);
		void parseIndex(ContIter &begin);
		void parseRoot(ContIter &begin);
		void parseLocation(ContIter &begin);
		void parseUploadDir(ContIter &begin);
		void parseAccessLog(ContIter &begin);

		std::string m_location;
		std::string m_root;
		std::string m_upload_dir;
		std::string m_access_log;
		std::list<std::string> m_indexes;
		bool m_autoindex;
};

#endif
