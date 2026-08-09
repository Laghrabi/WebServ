#include "webserver.hpp"

bool compare_header(const std::string& h1, const std::string& h2) {
	std::size_t size = h1.size();
	if (size != h2.size())
		return (false);
	for (std::size_t idx = 0; idx < size; idx++) {
		if (std::toupper(h1.at(idx)) != std::toupper(h2[idx])) {
			return (false);
		}
	}
	return (true);
}
