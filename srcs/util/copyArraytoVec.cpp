#include "webserver.hpp"

void copyArrayToVec(char* first, std::size_t n, std::vector<char>& vec)
{
	for (std::size_t i = 0; i < n; i++, ++first) {
        vec.push_back(*first);
	}
}
