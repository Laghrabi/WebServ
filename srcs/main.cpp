
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

			std::string request_str = "GET /home/zfarouk/Desktop/projects/WebServ/hey.out?hey=but&but=hey HTTP/1.1\r\nHOST: server2\r\nTransfer-Encoding: chunked\r\n\r\nb\r\nHello World\r\n0\r\n\r\n";

			HttpRequest request(range, Server::IPort());
			request.parse(std::vector<char>(request_str.begin(), request_str.end()));
			std::cout << "STATUS CODE: " << request.getStatusCode() << "\n";
			// std::cout << "SERVER ROOT: " << request

			if (request.getCurrentState() == FINISHED) {
				request.printBodyContent();
				std::cout << "===================this is routing phase=======================\n";
				conf.m_servers.clear();

				RouteManager m;
				RouteResult result = m.processRequest(request);
					std::cout << "status Code = " << result.statusCode << "\n";
				if (result.action != ACTION_ERROR) {
					std::cout << "target path = " << result.targetPath << "\n";
					std::cout << "action = " << result.action << "\n";

					CgiRequest cgi(request);
					cgi.printEnv();
				}
				else {
					HttpRequest::printHttpStatus(request.getStatusCode());
					std::cout << "error\n";
					// exit (5);
				}
				// manager.init();
				// manager.run();

			}
			else {
std::cout << "error request\n";
			}
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
