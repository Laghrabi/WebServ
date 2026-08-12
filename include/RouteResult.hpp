#ifndef _ROUTERESULT_H
#define _ROUTERESULT_H
#include "webserver.hpp"


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
	ACTION_UPLOAD_FILE,
	ACTION_ERROR,
	NONE
};


 struct CgiInfo {
	std::string pathInfo;
	std::string scriptName;
 };


 /**
 * @brief Encapsulates the result of the routing process.
 * * Contains the determined action, the physical target path (if applicable),
 * and the HTTP status code for the final response.
 */
struct RouteResult {
	RouteAction action;
	std::string targetPath;
	HttpStatus         statusCode;
	CgiInfo cgiInfo;
	const RouteConfig *route;
};

#endif
