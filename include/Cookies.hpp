#ifndef COOKIES_HPP
#define COOKIES_HPP

#include "webserver.hpp"

class Cookies
{
private:
    std::map<std::string, std::string> _cookies;

public:
    void parse(const std::string& header);

    bool has(const std::string& name) const;
    std::string get(const std::string& name) const;
};

#endif