#include "tokenization.hpp"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./program <file>\n";
        return 1;
    }

    try
    {
        std::vector<token> tokens = lexer::tokenizeFile(argv[1]);

        for (size_t i = 0; i < tokens.size(); i++)
        {
            std::cout << lexer::tokenTypeToString(tokens[i].type);

            if (!tokens[i].value.empty())
                std::cout << " -> \"" << tokens[i].value << "\"";

            std::cout << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}