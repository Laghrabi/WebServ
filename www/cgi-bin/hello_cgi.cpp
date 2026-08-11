#include<iostream>

int main(void){
	std::string response =
		"Content-Type:                     text/plain\n"
		"Content-Length: 13\n"
		"Connection: close\n"
		"\n"
		"Hello, world!";
	std::cout << response << "\n";
}
