#include "webserver.hpp"

int main(int argc, char **argv){
	(void)argc;
	(void)argv;
	HttpRequest request;	
	std::string request_str = "GET /hey/but%F3?hey=but&but=hey HTTP/1.0\r\n";
	std::vector<char> data(request_str.begin(), request_str.end());
	request.parse(data);
	CgiRequest cgi(request);
	cgi.printEnv();
	return (0);
}
