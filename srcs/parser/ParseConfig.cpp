#include "ParseConfig.hpp"
#include "Server.hpp"
#include "tokenization.hpp"
#include <exception>
#include <utility>




std::ostream& operator<<(std::ostream& out, const Server<std::vector<token> >::IPort& iport) {
	in_addr_t hey = iport.getAddr();
	char buffer[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &hey, buffer, INET_ADDRSTRLEN);
	return (out << buffer << ":" << iport.getPort());
}








