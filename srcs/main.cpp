
#include "Server.hpp"
#include "ParseConfig.hpp"
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
	// for (std::vector<token>::iterator it = tokens.begin(); it != tokens.end(); it++) {
	// 	std::cout << "[" <<  (*it).value << "]\n";
	// }

	ParseConfig<std::vector<token> > parser(tokens);
	try {
		parser.parse();
	}
	catch (const ParseConfig<TokenCont>::ConfigExcept& e) {
		std::cerr << e.what() << "\n";
	}

	return (0);
}
