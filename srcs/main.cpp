
#include "webserver.hpp"
#include "ConnectionManager.hpp"

void printRouteTree(RouteNode& route, int tabNum) {

	std::cout << std::string(tabNum, '\t') << "segment name = " << route.segmentName << "\n";
	if (route.config)
	 std::cout <<  std::string(tabNum, '\t') << " config = " << route.config->getRedirection().second << "\n";
	if (route.children.size() == 0)
		return ;
	for (std::map<std::string, RouteNode*>::const_iterator it = route.children.begin();
			it != route.children.end();
			++it) {
			// std::cout << std::string(tabNum, '\t') << "key = " << it->first << "\n";
			printRouteTree(*it->second, ++tabNum);
	}
}

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
			ConnectionManager manager(conf);

			// Config::ServerMultiMap& mymap = conf.m_iport_server;
			// for (Config::ServerMultiMap::iterator it = mymap.begin(); it != mymap.end(); ++it) {
			// 	it->second.buildRouteTree();
			// }
			// buildRouteTree((++mymap.begin())->second);
			//
			const Config::ServerMultiMap& map = conf.m_iport_server;
			Config::ServerRange range = map.equal_range(map.begin()->first);

			std::string request_str = "GET /home/hsacr/COMMON_CORE/webserver/tests/cgi/apache-cgi/cgi-bin/.something.hey.out/this/is/path/info HTTP/1.1\r\nHOST: server2\r\n\r\n";

			HttpRequest request(range);
			request.parse(std::vector<char>(request_str.begin(), request_str.end()));
			std::cout << "STATUS CODE: " << request.getStatusCode() << "\n";
			// std::cout << "SERVER ROOT: " << request

			conf.m_servers.clear();
			RouteManager m;
			// print(m.processRequest(request));
			std::cout << "target path is \"" << m.processRequest(request).targetPath << "\"\n";
			// std::cout << "action is \"" << m.processRequest(request).action << "\"\n";
			// std::cout << "status code is \"" << m.processRequest(request).statusCode << "\"\n";
			// ;

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
