
#include "HttpRequest.hpp"
#include "RouteManager.hpp"
#include "webserver.hpp"
#include <unistd.h>
#include "Config.hpp"
#include "ConnectionManager.hpp"
#include "ClientSocket.hpp"
#include "ConnectionManager.hpp"
#include "ListeningSocket.hpp"

// const RouteConfig* matchRoute(const std::vector<std::string>& uriSegments, const Server* server) {
//     if (!server)
//         return (NULL);
//     const RouteNode* currNode = &(server->m_route_tree);
//     const RouteConfig* bestMatch = server;
//
// 		std::cout << "server found " << server->getRedirection().second << "\n";
//     if (currNode->config)
//         bestMatch = currNode->config;
//
//     for (std::vector<std::string>::const_iterator it = uriSegments.begin(); it != uriSegments.end(); ++it) {
// 			std::cout << "\nsegment " << *it << "\n";
//         std::map<std::string, RouteNode*>::const_iterator match = currNode->children.find(*it);
//         if (match != currNode->children.end()) {
// 						// std::cout << "here " << currNode->config->getRedirection().second << "\n";
// 					// std::cout << "i find that " << *it << "\n";
//             currNode = match->second;
// 						// std::cout << "here " << currNode->config->getRedirection().second << "\n";
//             if (currNode->config)
//                 bestMatch = currNode->config;
//         } else {
//             break;
//         }
//     }
// 		// std::cout << "this is important " << bestMatch->getRedirection().second << "\n";
//
//     return (bestMatch);
// }
//
// static std::vector<std::string> tokenizeRoutePath(const std::string& path) {
// 	std::vector<std::string> tokens;
// 	std::string current = "";
//
// 	for (size_t i = 0; i < path.length(); ++i) {
// 		if (path[i] == '/') {
// 			if (!current.empty()) {
// 				tokens.push_back(current);
// 				current = "";
// 			}
// 		} else {
// 			current += path[i];
// 		}
// 	}
// 	if (!current.empty()) {
// 		tokens.push_back(current);
// 	}
// 	return tokens;
// }

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

// RouteNode buildRouteTree(Server& server) {
// 	std::vector<Location> m_locations = server.m_locations;	
// 	RouteNode m_route_tree("/");
// 	RouteNode* currentNode = &m_route_tree; 
//
// 	for (size_t i = 0; i < m_locations.size(); ++i) {
// 		std::vector<std::string> tokens = tokenizeRoutePath(m_locations[i].getPath());
//
// 		currentNode = &m_route_tree; 
//
// 		for (size_t j = 0; j < tokens.size(); ++j) {
// 			const std::string& token = tokens[j];
//
// 			if (currentNode->children.find(token) == currentNode->children.end()) {
// 				// currentNode->children.insert(std::make_pair(token, new RouteNode(token)));
// 				currentNode->children[token] = new RouteNode(token);
// 			}
// 			currentNode = currentNode->children[token];
// 		}
// 		// currentNode->config = new RouteConfig; 
// 		*currentNode->config = m_locations[i]; 
// 	}
// 	printRouteTree(m_route_tree, 1);
// 	return (m_route_tree);
// }

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
			std::string request_str = "GET /bin/ HTTP/1.1\r\nHOST: server3\r\n\r\n";
			HttpRequest request(range);
			request.parse(std::vector<char>(request_str.begin(), request_str.end()));
			std::cout << request.getStatusCode() << "\n"; 

			conf.m_servers.clear();
			RouteManager m;
			std::cout << "target path is \"" << m.processRequest(request).targetPath << "\"\n";
			std::cout << "action is \"" << m.processRequest(request).action << "\"\n";
			std::cout << "status code is \"" << m.processRequest(request).statusCode << "\"\n";
			;

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
