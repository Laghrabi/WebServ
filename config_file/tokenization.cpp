#include "tokenization.hpp"

token createToken(TokenType type, const std::string& value, int line)
{
    token tok;

    tok.type = type;
    tok.value = value;
    tok.line = line;

    return (tok);
}

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
            tokenz.push_back(createToken(OPEN_BRACE, "{", line));
            continue;
        }
        if (c == '}')
        {
            tokenz.push_back(createToken(CLOSE_BRACE, "}", line));
            continue;
        }
        if (c == ';')
        {
            tokenz.push_back(createToken(SEMICOLON, ";", line));
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
                std::stringstream ss;
                ss << "Unclosed quote at line " << startLine;
                throw std::runtime_error(ss.str());
            }
            tokenz.push_back(createToken(WORD, buffer, startLine));
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
        tokenz.push_back(createToken(WORD, buffer, line));
    }
    tokenz.push_back(createToken(END_OF_FILE, "", line));

    return tokenz;
}


std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
        case WORD:
            return "WORD";

        case OPEN_BRACE:
            return "OPEN_BRACE";

        case CLOSE_BRACE:
            return "CLOSE_BRACE";

        case SEMICOLON:
            return "SEMICOLON";

        case END_OF_FILE:
            return "EOF";
    }

    return "UNKNOWN";
}

std::vector<token> tokenization(const std::string& file_name)
{
    std::ifstream file(file_name.c_str());

    if (!file)
        throw std::runtime_error("failed to open file");

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string content = buffer.str();

    lexer lex(content);

    return lex.tokenization();
}



int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./program <file>\n";
        return 1;
    }
    try
    {

        std::vector<token> tokens = tokenization(argv[1]);

        /* just for testing */
        for (size_t i = 0; i < tokens.size(); i++)
        {
            std::cout << tokenTypeToString(tokens[i].type);

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