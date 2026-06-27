#include<iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <stdio.h>

int main(void){
	sockaddr addr_in;
	// addr_in.sin_addr.s_addr = htonl(127<<0);
	// addr_in.sin_port = htons(80);
	// addr_in.sin_family = AF_INET;
	// std::memset(addr_in.sin_zero, 0, sizeof(addr_in.sin_zero));

	struct addrinfo hints;
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET6;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
  hints.ai_flags = 0; 
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	sockaddr_in hey;
	hey.sin_addr;
	hey.sin_family;
	hey.sin_port;

	struct addrinfo *ptr;
	std::cout << (sizeof (sockaddr)) << "\n";
	std::cout << sizeof (sockaddr_in) << "\n";
	std::cout << sizeof(sockaddr_in6) << "\n";

	int fail = getaddrinfo("www.google.com", "lajsdlfaslkdjf", &hints, &ptr);
	if (fail)
	{
		perror("getaddrinfo");
		std::cout << fail << "\n";
		std::cout << gai_strerror(fail);
		exit (2);
	}
	sockaddr_in h;
	sockaddr_in b;
	sockaddr *m = reinterpret_cast<sockaddr*>(new sockaddr_in);
	std::cout << (h == b);
	addrinfo *tmp = ptr;
	for (; tmp != NULL; tmp = tmp->ai_next)
	{
		sockaddr_in6* hey = reinterpret_cast<sockaddr_in6 *>(tmp->ai_addr);
		std::cout << "port = " << ntohs(hey->sin6_port) << "\n";
		char buffer[INET6_ADDRSTRLEN] =  {0};

		const char *addr_str = inet_ntop(AF_INET6, &hey->sin6_addr, buffer, INET6_ADDRSTRLEN);
		if (!addr_str) {
			perror("inet_ntop");
			exit (2);
		}
		std::string command = std::string("zen ") + addr_str;
		std::cout << buffer << "\n";
	}
	freeaddrinfo(ptr);

	return (0);
}
