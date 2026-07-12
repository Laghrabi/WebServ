
#include "webserver.hpp"
#include <unistd.h>
#include "Config.hpp"
#include "ConnectionManager.hpp"
#include "ClientSocket.hpp"
#include "ConnectionManager.hpp"
#include "ListeningSocket.hpp"

// void printQueryParams( const std::multimap<std::string, std::string> _queryParams) {
//     std::cout << "--- Query Parameters ---\n";
    
//     for (std::multimap<std::string, std::string>::const_iterator it = _queryParams.begin(); 
//          it != _queryParams.end(); 
//          ++it) {
        
//         std::cout << it->first << " = " << it->second << "\n";
//     }
    
//     std::cout << "------------------------\n";
// }

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
			// HttpRequest request;
			ConnectionManager manager(conf);
			// print(conf);
			
			
			
			const UnorderedMultiMap<Server::IPort, Server>& mymap = conf.m_iport_server;
			
			for (UnorderedMultiMap<Server::IPort, Server>::const_iterator it = mymap.begin(); it != mymap.end(); it = mymap.upper_bound(it->first)) {	
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
			manager.init();
			manager.run();

			// std::string rawString = 
        	// "GET /api/users/../../../pass/?name=admin&sort=asc&name=chaimaa HTTP/1.0\r\n"
        	// "Host: localhost:8080\r\n"
        	// "User-Agent: curl/7.68.0\r\n"
        	// "Accept: */*\r\n"
        	// "\r\n";
			// std::vector<char> rawBuffer(rawString.begin(), rawString.end());
			// request.parse(rawBuffer);

			// std::cout << "State       : " << request.getCurrentState() << " (FINISHED = 6, ERROR = 7)\n";
    		// std::cout << "Status Code : " << request.getStatusCode() << "\n";
    		// std::cout << "Method      : [" << request.getMethod() << "]\n";
    		// std::cout << "Route URI   : [" << request.getRouteUri() << "]\n";
    		// std::cout << "Query String: [" << request.getQueryString() << "]\n";
    		// std::cout << "Version     : [" << request.getVersion() << "]\n";
			// printQueryParams(request.getQueryParams());

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
