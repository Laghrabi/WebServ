
#include "MimeTypesExt.hpp"
#include "Server.hpp"
#include "ParseConfig.hpp"
#include "webserver.hpp"
#include <unistd.h>
#include "Config.hpp"
#include "ConnectionManager.hpp"

int main(int argc, char **argv){
	if (argc != 2)
	{
		std::cerr << "./webserver configfile.conf";
		return (1);
	}

	std::ifstream file(argv[1]);

	if (!file)
	{
		std::cerr << "failed to open file\n";
		return (1);
	}
	try {
		std::vector<token> tokens = lexer::tokenizeFile(argv[1]);

		ParseConfig parser(tokens);
		try {
			Config conf = parser.parse();
			ConnectionManager manager(conf);
			manager.init();
			manager.run();
			// print(conf);

		

			UnorderedMultiMap<Server::IPort, Server>& mymap = conf.m_iport_server;

			for (UnorderedMultiMap<Server::IPort, Server>::const_iterator it = mymap.begin(); it != mymap.end(); ++it) {
				std::cout << "new iport: " << it->first << "\n";
			}
			// std::multimap<Server::IPort, Server> hey = conf.m_iport_server;
			//
			// for (std::multimap<Server::IPort, Server>::const_iterator it = hey.begin(); it != hey.end();) {
			// 	const Server::IPort& iport =  it->first;
			// 	// std::cout << iport.getPort() << "\n";
			// 	it = hey.upper_bound(iport);
			// 	// sleep (1);
			// }
			for (std::list<Server>::iterator it = conf.m_servers.begin(); it != conf.m_servers.end(); ++it) {
    			it->buildRouteTree();
			}
		}
		catch (const ParseConfig::ConfigExcept& e) {
			std::cerr << e.what() << "\n";
		}
	}
	catch (const std::exception& e){

	}
	
	

	return (0);
}
