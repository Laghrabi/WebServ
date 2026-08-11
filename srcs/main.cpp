
#include "webserver.hpp"
#include "ConnectionManager.hpp"
#include "CgiHandler.hpp"

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
	if (argc > 2)
	{
		std::cerr << "./webserver configfile.conf";
		return (1);
	}
	try {
		std::vector<token> tokens;
		if (argc == 2)
			tokens = lexer::tokenizeFile(argv[1]);
		else
			tokens = lexer::tokenizeFile("test_config/website.conf");
		ParseConfig parser(tokens);
		Config conf = parser.parse();
		ConnectionManager manager(conf);
		manager.init();
		manager.run();

	}
	catch (const std::exception& e){
		std::cerr << e.what() << "\n";
		return (1);
	}



	return (0);
}
