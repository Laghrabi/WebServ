#ifndef _REQUESTINFO_H
#define _REQUESTINFO_H
#include "webserver.hpp"
#include <exception>

class ParseConfig;

/* NOTE: 
 * route config has all things need for routing:
 *	-> root directory
 *	-> upload dir: where to put any upload file
 *	-> access log: file where to put log
 *	-> autoindex can i requst a url like var/data/www/ where www is directory
 *	so if autoindex is on i can get a file(index) or the webserver can create a default page
 *	like (firefox does when just typing '/')
 *	-> indexes: if autoindex is on there is a list of files
 *
 * INFO: there is getters for all members just use getters since the members(attributes)
 * are private
 */

class RouteConfig {

	protected:
		typedef std::vector<token> Container;
		typedef Container::const_iterator ContIter;

		typedef void (RouteConfig::*HandlerFunc)(ContIter&);
		typedef std::map<std::string, HandlerFunc> MapHandler ;
		typedef ParseConfig ParseConfigType ;

		static MapHandler s_handlers;

		void addMethod(const std::string& new_method) throw (std::exception);
		const std::set<std::string>& getAllowedMethods() const;
		void parseCgiConf(ContIter &begin);
		bool validExtention(const std::string& ext, std::string& err_msg);

	public:
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

		void initAvailableMethods();


		// getters
		const std::string& getRoot() const;
		const std::string& getUploadDir() const;
		const std::string& getAccessLog() const;
		const std::list<std::string>& getIndexes() const;
		bool isAutoindex() const;
		std::size_t getMaxBodySize() const;
		bool hasMaxBodySize() const;

		// check if the method is allowed
		bool isAllowed(const std::string& method) const;




	private:
		std::string m_root;
		std::string m_upload_dir;
		std::string m_access_log;
		std::list<std::string> m_indexes;
		bool m_autoindex;
		std::size_t m_max_body_size;
		bool m_max_body_size_exist;
		std::set<std::string> m_allowed_methods;
		std::set<std::string> m_cgi_map;

		static std::set<std::string> s_available_methods;
};


#endif
