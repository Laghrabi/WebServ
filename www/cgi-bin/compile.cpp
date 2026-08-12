#include <fstream>
#include<iostream>
#include<algorithm>
#include <ostream>

void print_env(char **envp) {
	while (*envp) {
		printf("<div>%s</div><br>\n", *envp);
		envp++;
	}
}

int main(int argc, char **argv, char **envp){
	printf("Content-Type: text/html\n\n");
	// print_env(envp);
	std::ofstream file("./file.c");
	std::string buff;
	int i = 0;
	while (std::getline(std::cin, buff, '\n').good()) {
		if (!buff.empty())
		{
			file << buff << std::endl;
		}
		buff.clear();
		++i;
	}
	if (!buff.empty())
		file << buff << std::flush;
	// std::cout << "hello\n" << std::endl;
	std::string command = "docker build -t compile -f compile.dockerfile . 2> /dev/null  && docker run -t compile ";
	system(command.c_str());
	return (0);
}
