#include "../../include/webserver.hpp"
#include <map>
int main() {
	typedef UnorderedMultiMap<int, std::string> type;
	UnorderedMultiMap<int, std::string> h;
	// h.insert(1, "1");
	// h.insert(1, "1");
	// h.insert(1, "1");
	// h.insert(2, "hey");
	// h.insert(3, "hey");
	// h.insert(4, "hey");
	h.insert(5, "o");
	h.insert(5, "5 dogs");
	h.insert(5, "5 cats");
	h.insert(5, "5 five");
	h.insert(5, "hey");
 const UnorderedMultiMap<int, std::string> l =h;
	for (type::const_iterator it = l.begin(); it != l.end(); ++it) {
		// it->second = "but";
	}
	l.print();
	for (type::const_iterator it = l.begin(); it != l.end(); ++it) {
		std::cout << "first = " << it->first <<" it->second = " << it->second << "\n";
	}
	std::cout << "\n";

for (type::const_iterator it = l.begin(); it != l.end(); it = l.upper_bound(it->first)) {
		std::cout << "first = " << it->first <<" it->second = " << it->second << "\n";
		(*it);
	}

	std::pair<type::iterator, type::iterator> p = h.equal_range(5);

	std::cout << "\n\n";
for (type::iterator it = p.first; it != h.end(); it = ++it) {
		std::cout << "first = " << *it <<" it->second = " << it->second << "\n";
	}
}
