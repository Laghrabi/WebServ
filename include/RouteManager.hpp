#ifndef ROUTE_MANAGER_HPP
# define ROUTE_MANAGER_HPP

# include "webserver.hpp"

class HttpRequest;

/**
 * @brief Defines the set of possible actions for the request handler.
 * * Each action corresponds to a specific server response behavior, 
 * ranging from file serving to CGI execution or error reporting.
 */
enum RouteAction {
	ACTION_SERVE_FILE,
	ACTION_SERVE_INDEX,
	ACTION_AUTOINDEX,
	ACTION_EXECUTE_CGI,
	ACTION_REDIRECT,
	ACTION_ERROR,
	NONE
};

/**
 * @brief Encapsulates the result of the routing process.
 * * Contains the determined action, the physical target path (if applicable),
 * and the HTTP status code for the final response.
 */
struct RouteResult {
	RouteAction action;
	std::string targetPath;
	int         statusCode;
};

class RouteManager {
	private:
		ResourceLocator _locator;

	public:
		RouteManager();
		RouteManager(const RouteManager& other);
		RouteManager& operator=(const RouteManager& other);
		~RouteManager();


		bool isCgi(const std::vector<std::string>& script_path, const RouteConfig* route, const std::string& location) const;
		RouteResult processRequest(const HttpRequest& request) const;
		const RouteConfig* matchRoute(const std::vector<std::string>& uri, const Server* server, std::string& location) const;
};

#endif
