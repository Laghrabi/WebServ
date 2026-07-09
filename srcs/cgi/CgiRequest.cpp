#include "./CgiRequest.hpp"
#include "HttpRequest.hpp"
using std::string;

void CgiRequest::setRequestEnv(const HttpRequest& request) {
	string uri = request.getUri();
	string method = request.getMethod();
	string querry_string = request.getQueryString();
}

void CgiRequest::setEnv(void) {
	;	
}
