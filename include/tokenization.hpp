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
    END_OF_FILE
};

struct token
{
    TokenType        type;
    std::string value;
    int     line;
		bool operator==(TokenType t) {
			return (type == t);
		}
		bool operator==(std::string str) {
			return (value == str);
		}

		bool is(TokenType t) const {
			return (type == t);
		}
		bool is(std::string str) const {
			return (value == str);
		}
		bool is_eof() const {
			return (type == END_OF_FILE);
		}
};

class lexer
{
    private:
        std::string input;
        size_t pos;
        int line;
    
    public:
				typedef std::vector<token> Container;
				typedef std::vector<token>::const_iterator ContIter;
        lexer(const std::string& content): input(content), pos(0), line(1) {};
        std::vector<token> tokenization();
        static std::vector<token> tokenizeFile(const std::string& file_name);
        static token createToken(TokenType type, const std::string& value, int line);
        static std::string tokenTypeToString(TokenType type);
};

#endif 
