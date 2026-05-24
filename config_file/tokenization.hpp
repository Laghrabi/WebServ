#ifndef TOKENIZATION_HPP
#define TOKENIZATION_HPP

#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include <sstream>
#include <string>

enum TokenType
{
    WORD,
    OPEN_BRACE,
    CLOSE_BRACE,
    SEMICOLON,
    END_OF_FILE
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
};

#endif 
