#include "tokenization.hpp"

/**
 * @brief Demonstration entry point that tokenizes a configuration file.
 *
 * The program expects a single file path argument, prints each token to
 * standard output, and returns a non-zero exit code on error.
 *
 * @param argc Number of command-line arguments.
 * @param argv Command-line argument vector.
 * @return 0 on success, 1 on invalid usage or runtime failure.
 */
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