
#include"webserver.hpp"

int main(void){
	UnorderedMultiMap<std::string, int> p;
	p.insert("hey", 20);
	p.insert(std::make_pair("carrot", 200));
	p.insert(std::make_pair("carrot", 20));
	typedef UnorderedMultiMap<std::string, int>::iterator mapIter;
//  mapIter it = p.begin();
// while (it != p.end()) {
// 	std::cout << it->first << ": " << it->second << "\n";
// 	it++;
// }

std::pair<UnorderedMultiMap<std::string, int>::iterator, UnorderedMultiMap<std::string, int>::iterator > hey = p.equal_range("carrot");
while (hey.first != hey.second) {
	std::cout << hey.first->first << hey.first->second << "\n";
	hey.first++;
}
	return (0);
}
