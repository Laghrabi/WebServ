#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>

int main(void){
	printf("Content-Type: text/html\n\n");
	printf("hello from a container\n");
	system("cat /etc/os-release");
	return (0);
}
