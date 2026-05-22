#ifndef TOKENIZATION_HPP
#define TOKENIZATION_HPP

#include <iostream>
#include <fstream>
#include <exception>
#include <vector>
#include <sstream>

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
        int pos;
        int line;
    
    public:
        lexer(std::string& content): input(content), pos(0), line(1) {};
        std::vector<token> tokenization();
};

#endif 
