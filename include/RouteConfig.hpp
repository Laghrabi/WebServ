#ifndef _REQUESTINFO_H
#define _REQUESTINFO_H
#include "webserver.hpp"
#include <exception>

class ParseConfig;

class RouteConfig {
	// class HttpMethods {
	// 	public:
	// 	HttpMethods();
	// };

	public:
	typedef std::vector<token> Container;
	typedef Container::iterator ContIter;

	typedef void (RouteConfig::*HandlerFunc)(ContIter&);
	typedef std::map<std::string, HandlerFunc> MapHandler ;
	typedef ParseConfig ParseConfigType ;

	static MapHandler s_handlers;

	RouteConfig();

	static HandlerFunc getDirectiveHandler(const std::string dir_name);
	static void init(void);

	void parseAutoIndex(ContIter &begin);
	void parseIndex(ContIter &begin);
	void parseRoot(ContIter &begin);
	// void parseRouteConfig(ContIter &begin);
	void parseUploadDir(ContIter &begin);
	void parseAccessLog(ContIter &begin);
	void parseMaxBodySize(ContIter &begin);
	void parseAllowedMethods(ContIter &begin);
	bool isAllowed(const std::string& method);

	void initAvailableMethods();

	void addMethod(const std::string& new_method) throw (std::exception);


	std::string m_location;
	std::string m_root;
	std::string m_upload_dir;
	std::string m_access_log;
	std::list<std::string> m_indexes;
	bool m_autoindex;
	std::size_t m_max_body_size;
	bool m_max_body_size_exist;
	std::string upload_dir;
	std::set<std::string> m_allowed_methods;

	private:
	static std::set<std::string> s_available_methods;
};


#endif
