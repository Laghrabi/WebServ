#include<iostream>
#include<unistd.h>

int main(void){
	std::cout << "content-type: text/html\n\n";
	// sleep(10);
	while (true) {
		std::cout << "hello\n";
	}
		// std::cout << std::string(100000, 'b');
	return (0);
}
