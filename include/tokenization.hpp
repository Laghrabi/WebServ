#ifndef TOKENIZATION_HPP
#define TOKENIZATION_HPP

#include <cstddef>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

enum TokenType
{
    WORD,
    OPEN_BRACE,
    CLOSE_BRACE,
    SEMICOLON,
    // END_OF_FILE
};

struct token
{
    TokenType        type;
    std::string value;
    int     line;
};

class lexer
{
    private:
        std::string input;
        size_t pos;
        int line;
    
    public:
        lexer(const std::string& content): input(content), pos(0), line(1) {};
        std::vector<token> tokenization();
        static std::vector<token> tokenizeFile(const std::string& file_name);
        static token createToken(TokenType type, const std::string& value, int line);
        static std::string tokenTypeToString(TokenType type);
};

#endif 
