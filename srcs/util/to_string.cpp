#include "webserver.hpp"

std::string to_string(std::size_t num) {
	std::stringstream ss;
	ss << num;
	std::string res = ss.str();
	return (res);
}
