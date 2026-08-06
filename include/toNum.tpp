#include "webserver.hpp"


template <typename T> bool toNum(T &num, const std::string& str) {
	size_t found = str.find_first_not_of("+-0123456789");
	if (found != std::string::npos) {
		return (false);
	}
	std::stringstream ss;

	ss << str;

	if (!(ss >> num)) {
		return false;
	}
	return true;
}
