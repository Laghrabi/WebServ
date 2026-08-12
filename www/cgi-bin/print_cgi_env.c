#include <stdio.h>

int main(int argc, char **argv, char **envp) {
	// char buff[5] = {0};
	// read(0, buff, 4);
	// exit(5);
	// for (int i =0; i < 500; i++) {
	// 	printf("somethingelese%d: text/html\r\n", i);
	// 	printf("something%d: text/html\r\n", i);
	// }

		// printf("something%d: text/html", 20);
		printf("Content-Type: text/html\n\n");
//printf("Content-Length: 10\r\n\r\n");

	while (*envp) {
		printf("<div>%s</div><br>\n", *envp);
		envp++;
	}
	// printf("%s\n", buff);
}
