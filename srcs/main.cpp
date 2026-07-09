
#include "HttpRequest.hpp"
#include "webserver.hpp"
#include <unistd.h>
#include "Config.hpp"
#include "ConnectionManager.hpp"
#include "ClientSocket.hpp"
#include "ConnectionManager.hpp"
#include "ListeningSocket.hpp"

int main(int argc, char **argv){
	(void)argc;
	(void)argv;
	HttpRequest request;	
	std::string request_str = "GET /hey/but%F3?hey=but&but=hey\r\n";
	std::vector<char> data(request_str.begin(), request_str.end());
	request.parse(data);
	// method uri name query_string
	std::cout << request.getUri() << "\n";
	std::cout << request.getMethod() << "\n";
	std::cout << request.getQueryString();
	typedef std::multimap<std::string, std::string> type;
	type m = request.getQueryParams();
	std::cout << m.size() << '\n';
	for (type::iterator it = m.begin(); it != m.end(); ++it) {
		std::cout << it->first << '\n';
	}

	return (0);
}
