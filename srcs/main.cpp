#include "webserver.hpp"

int main(int argc, char **argv){
	if (argc != 2)
	{
		std::cerr << "./webserver configfile.conf";
		return (1);
	}
	lexer lex(argv[1]);
	std::vector<token> tokens = lex.tokenization();
	return (0);
}
