#include "ParseConfig.hpp"
#include "Server.hpp"
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
	std::vector<token> tokens = lexer::tokenizeFile(argv[1]);
	// print(tokens);

	ParseConfig<std::vector<token> > parser(tokens);
	parser.parse();

	// Server::parseIPort("1.1.1.1:80");
	return (0);
}
