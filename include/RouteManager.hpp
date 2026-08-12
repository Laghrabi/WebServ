#ifndef ROUTE_MANAGER_HPP
# define ROUTE_MANAGER_HPP

# include "webserver.hpp"
# include "ResourceLocator.hpp"
# include "RouteResult.hpp"

class HttpRequest;

class RouteManager {
	private:
		ResourceLocator _locator;
		std::string _basePath;
		std::string _resource;
		void checkPostPath(const std::string& Path, HttpRequest& request);
		
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
