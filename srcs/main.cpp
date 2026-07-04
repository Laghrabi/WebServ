
#include "webserver.hpp"

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
			const Config conf = parser.parse();
			// print(conf);

		// for (std::list<Server>::iterator it = conf.m_servers.begin(); it != conf.m_servers.end(); ++it) {
		//   		it->buildRouteTree();
		// }

			// typedef UnorderedMultiMap<Server::IPort, Server> type;
			const UnorderedMultiMap<Server::IPort, Server>& mymap = conf.m_iport_server;

			for (UnorderedMultiMap<Server::IPort, Server>::const_iterator it = mymap.begin(); it != mymap.end(); it = mymap.upper_bound(it->first)) {
				// it->first = "hey";
				std::cout << "new iport: " << it->first << "\n";

				std::pair<UnorderedMultiMap<Server::IPort, Server>::const_iterator, UnorderedMultiMap<Server::IPort, Server>::const_iterator> pair = mymap.equal_range(it->first)	;

				UnorderedMultiMap<Server::IPort, Server>::const_iterator begin = pair.first;
				UnorderedMultiMap<Server::IPort, Server>::const_iterator end = pair.second;

				for (;begin != end; ++begin) {
					Server myserver = begin->second;
				}
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
