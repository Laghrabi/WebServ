
#include "MimeTypesExt.hpp"
#include "Server.hpp"
#include "ParseConfig.hpp"
#include "webserver.hpp"
#include <unistd.h>

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
			print(conf);

		for (std::list<Server>::iterator it = conf.m_servers.begin(); it != conf.m_servers.end(); ++it) {
    		it->buildRouteTree();
		}

			UnorderedMultiMap<Server::IPort, Server>& mymap = conf.m_iport_server;

			for (UnorderedMultiMap<Server::IPort, Server>::iterator it = mymap.begin(); it != mymap.end(); it.nextBound()) {
				std::cout << "key = " << it.getKey() << "\n";
				sleep(1);
			}
			// std::multimap<Server::IPort, Server> hey = conf.m_iport_server;
			//
			// for (std::multimap<Server::IPort, Server>::const_iterator it = hey.begin(); it != hey.end();) {
			// 	const Server::IPort& iport =  it->first;
			// 	// std::cout << iport.getPort() << "\n";
			// 	it = hey.upper_bound(iport);
			// 	// sleep (1);
			// }
		}
		catch (const ParseConfig::ConfigExcept& e) {
			std::cerr << e.what() << "\n";
		}
	}
	catch (const std::exception& e){

	}
	Location hey;

	UnorderedMultiMap<int, std::string> h;
	h.insert(2, "hey");
	h.insert(2, "heyb");
	h.insert(1, "heyb");
	h.insert(2, "heyb");
	h.insert(2, "heyb");
	h.insert(5, "5");
	h.insert(6, "6");
	h.insert(10, "10");
	h.insert(10, "10");
	h.insert(10, "10");
	h.insert(10, "10");


	for (UnorderedMultiMap<int, std::string>::iterator it = h.begin(); it != h.end(); it.nextBound()) {
			std::cout << "key = " << it.getKey() << "\n";
			sleep(1);
	}

	return (0);
}
