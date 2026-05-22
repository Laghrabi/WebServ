#include "tokenization.hpp"

std::vector<token> lexer::tokenization()
{
    std::vector<token> tokenz;
    for (; pos < input.size();  pos++)
    {
        char c = input[pos];
        if (std::isspace(c))
        {
            if (c == '\n')
                line++;
            continue;
        }
        if (c == '#')
        {
            while (pos < input.size() && input[pos] != '\n')
                pos++;
            pos--;
            continue;
        }
        if (c == '{')
        {
            tokenz.push_back({OPEN_BRACE, "{", line});
            continue;
        }
        if (c == '}')
        {
            tokenz.push_back({CLOSE_BRACE, "}", line});
            continue;
        }
        if (c == ';')
        {
            tokenz.push_back({SEMICOLON, ";", line});
            continue;
        }
        if (c == '"')
        {
            int startLine = line;
            pos++;
            std::string buffer;
            while (pos < input.size() && input[pos] != '"')
            {
                if (input[pos] == '\n')
                    line++;
                buffer += input[pos];
                pos++;
            }
            if (pos >= input.size())
            {
                throw std::runtime_error("Unclosed quote at line " + std::to_string(startLine));
            }
            tokenz.push_back({WORD, buffer, startLine});
            continue;
        }
        std::string buffer;
        while (pos < input.size())
        {
            char current = input[pos];

            bool isSpecial =
                std::isspace(current) ||
                current == '{' ||
                current == '}' ||
                current == ';' ||
                current == '#';

            if (isSpecial)
                break;

            buffer += current;
            pos++;
        }
        pos--;
        tokenz.push_back({WORD, buffer, line});
    }
    tokenz.push_back({END_OF_FILE, "", line});

    return tokenz;
}


std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
        case WORD:
            return "WORD";

        case TokenType::OPEN_BRACE:
            return "OPEN_BRACE";

        case TokenType::CLOSE_BRACE:
            return "CLOSE_BRACE";

        case TokenType::SEMICOLON:
            return "SEMICOLON";

        case TokenType::END_OF_FILE:
            return "EOF";
    }

    return "UNKNOWN";
}

int tokenization(const std::string& file_name)
{
    std::ifstream file(file_name);

    if (!file)
    {
        std::cerr << "failed to open file\n";
        return (1);
    }
    std::stringstream buffer;
    std::string content;
    buffer << file.rdbuf();
    content = buffer.str();
    
    try
    {
        lexer lexer(content);

        std::vector<token> tokens = lexer.tokenization();

        /*just for testing*/ 
        // for (size_t i = 0; i < tokens.size(); i++)
        // {
        //     std::cout << tokenTypeToString(tokens[i].type);
        //     if (!tokens[i].value.empty())
        //         std::cout << " -> \"" << tokens[i].value << "\"";
        //     std::cout << std::endl;
        // }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Lexer error: " << e.what() << '\n';
        return (1);
    }
    return (0);
}
