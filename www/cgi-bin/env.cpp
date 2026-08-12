#include<iostream>

int main(int argc, char **argv, char **envp){
	std::cout << "Content-Type: application/html\n\n";
	while (*envp) {
		std::cout << "<div>"<<  *envp << "</div>\n";
		envp++;
	}
	return (0);
}
