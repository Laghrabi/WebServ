#ifndef ROUTE_MANAGER_HPP
# define ROUTE_MANAGER_HPP

# include "webserver.hpp"
# include "ResourceLocator.hpp"
# include "RouteResult.hpp"

class HttpRequest;

/**
 * @brief Defines the set of possible actions for the request handler.
 * * Each action corresponds to a specific server response behavior, 
 * ranging from file serving to CGI execution or error reporting.
 */


/**
 * @brief Encapsulates the result of the routing process.
 * * Contains the determined action, the physical target path (if applicable),
 * and the HTTP status code for the final response.
 */



class RouteManager {
	private:
		ResourceLocator _locator;
		std::string _basePath; // if uri is /bin/bash and root is /usr/ /bin/ is location so _basePath = /usr/bin/
													 // if it is aliased to ur/bin/ so _basePath = /usr/bin/
		std::string _resource;

	public:
		RouteManager();
		RouteManager(const RouteManager& other);
		RouteManager& operator=(const RouteManager& other);
		~RouteManager();


		bool isCgi(const std::vector<std::string>& script_path, RouteResult& result,const std::string& location) const;
		static void printRouteAction(RouteAction action);

		void setResult(HttpStatus state, RouteAction action, std::string path, RouteResult& result);
		std::string resolveUploadPath(const std::string& uri, const std::string& locationMatch, const std::string& uploadStore);
		void determineResourceAction(RouteResult& result, ResourceType type, const std::string& physicalPath, const std::string& routeUri);
		void processRequest(HttpRequest& request) ;
		const RouteConfig* matchRoute(const std::vector<std::string>& uri, const Server* server, std::string& location) ;
};

#endif
