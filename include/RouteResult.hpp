#ifndef _ROUTERESULT_H
#define _ROUTERESULT_H
#include "webserver.hpp"

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

struct RouteResult {
	RouteAction action;
	std::string targetPath;
	int         statusCode;
	CgiInfo cgiInfo;
	const RouteConfig *route;
};

#endif
