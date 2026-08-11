#include "webserver.hpp"

std::string to_string(int num) {
	std::stringstream ss;
	ss << num;
	std::string res = ss.str();
	return (res);
}
