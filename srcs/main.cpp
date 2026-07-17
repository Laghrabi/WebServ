
#include "HttpRequest.hpp"
#include "RouteManager.hpp"
#include "webserver.hpp"
#include <unistd.h>
#include "Config.hpp"
#include "ConnectionManager.hpp"
#include "ClientSocket.hpp"
#include "ConnectionManager.hpp"
#include "ListeningSocket.hpp"
int main(int argc, char **argv){
	if (argc != 2)
	{
		std::cerr << "./webserver configfile.conf";
		return (1);
	}
	try {
		std::vector<token> tokens = lexer::tokenizeFile(argv[1]);

		ParseConfig parser(tokens);
		try {
			Config conf = parser.parse();
			// HttpRequest request;
			ConnectionManager manager(conf);
			// print(conf);

			// Config::ServerMultiMap& mymap = conf.m_iport_server;
			// for (Config::ServerMultiMap::iterator it = mymap.begin(); it != mymap.end(); ++it) {
			// 	it->second.buildRouteTree();
			// }
			const Config::ServerMultiMap& map = conf.m_iport_server;
			Config::ServerRange range = map.equal_range(map.begin()->first);
			std::string request_str = "GET /heys/ HTTP/1.1\r\nHOST: server3\r\n\r\n";
			HttpRequest request(range);
			request.parse(std::vector<char>(request_str.begin(), request_str.end()));
			std::cout << request.getStatusCode() << "\n"; 

			RouteManager m;
			std::cout << "target path == " << m.processRequest(request).targetPath << "\n";
			std::cout << "status code = " << m.processRequest(request).statusCode << "\n";


			// manager.init();
			// manager.run();

			}
			catch (const ParseConfig::ConfigExcept& e) {
				std::cerr << e.what() << "\n";
			}
	}
	catch (const std::exception& e){
		std::cerr << e.what() << "\n";
		return (1);
	}



	return (0);
}
