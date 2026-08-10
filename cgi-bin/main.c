#include<stdio.h>
#include<stdlib.h>

int main(void){
	printf("hello %s\n", getenv("USER"));
	system("cat /etc/passwd");
	return (0);
}
