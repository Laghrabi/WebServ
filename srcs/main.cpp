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

	std::stringstream buffer;
	buffer << file.rdbuf();
	lexer lex(buffer.str());
	std::vector<token> tokens = lex.tokenization();
	print(tokens);

	ParseConfig parser(tokens);

	Server::ParseServer::parseIPort("1.1.1.1:80");
	return (0);
}
