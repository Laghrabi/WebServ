#include "tokenization.hpp"

/**
 * @brief Creates a token object from its components.
 *
 * @param type Token category to assign.
 * @param value Token text payload.
 * @param line Source line where the token was recognized.
 * @return A populated token instance.
 */
token lexer::createToken(TokenType type, const std::string& value, int line)
{
    token tok;

    tok.type = type;
    tok.value = value;
    tok.line = line;

    return (tok);
}

/**
 * @brief Scans the input buffer and produces a token stream.
 *
 * The lexer skips whitespace, ignores shell-style comments beginning with
 * '#', handles quoted strings as single tokens, and emits structural tokens
 * for braces and semicolons.
 *
 * @return Token sequence terminated by an EOF token.
 * @throws std::runtime_error If a quoted string reaches EOF without closing.
 */
std::vector<token> lexer::tokenization()
{
    std::vector<token> tokenz;
    for (; pos < input.size();  pos++)
    {
        char c = input[pos];
        if (std::isspace(static_cast<unsigned char>(c)))
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
        if (c == '"' || c == '\'')
        {
            char quote = c;
            int startLine = line;
            pos++;
            std::string buffer;
            while (pos < input.size() && input[pos] != quote)
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
                std::isspace(static_cast<unsigned char>(current)) ||
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

/**
 * @brief Converts a token type to a readable name.
 *
 * @param type Token category to convert.
 * @return String name for the supplied token type.
 */
std::string lexer::tokenTypeToString(TokenType type)
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

/**
 * @brief Opens a file, reads its contents, and tokenizes the result.
 *
 * @param file_name Path to the file to tokenize.
 * @return Token sequence generated from the file contents.
 * @throws std::runtime_error If the file cannot be opened.
 */
std::vector<token> lexer::tokenizeFile(const std::string& file_name)
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
