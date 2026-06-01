#include "webserver.hpp"
#include "Server.hpp"
#include <arpa/inet.h>

// Server::Server() {
//
// }

// void Server::parseIPort(Server& server, std::vector<token>::iterator &it) {
//   std::string iport_str = (*it).value;
//   size_t pos;// = iport_str.find(":");
//   IPort iport(Server::default_ip, Server::default_ip);
//   std::string ip;
//   std::string port;
//
//   if ((pos = iport_str.find(":")) != std::string::npos) {
//     ip = iport_str.substr( 0, pos);
//     port = iport_str.substr(pos + 1);
//     iport.setPort(ip);
//     iport.setPort(port);
//
//     std::cout << ip;
//     std::cout << "\n";
//     std::cout << port;
//
//   }
//   else if (iport_str.find(".") != std::string::npos) {
//     iport.setIp(iport_str);
//   }
//   else {
//     iport.setPort(iport_str);
//   }
//   server.m_addr.push_back(iport);
// }

// void Server::IPort::setIp(const std::string& ip) throw(std::exception) {
//   struct in_addr addr;
//   int sucess;
//
//   sucess = inet_pton(AF_INET, ip.c_str(), &addr);
//   if (sucess == 1){
//     m_addr_ip = addr.s_addr;	
//   }
//   else {
//     if (sucess == 0)	
//     {
//       throw (std::exception());
//     }
//     else {
//       perror("error inet_pton");
//       throw (std::exception());
//     }
//   }
// }
//
// void Server::IPort::setPort(const std::string& port) throw(std::exception) {
//   std::stringstream ss(port);
//   ss >> m_port;
//   // if (ss.fail() || ss.tellg() != port.length()	)
//   // 	throw (std::exception());
// }
//
// void Server::IPort::setPort(in_port_t port) {
//   m_port = port;
// }
//
// void Server::IPort::setIp(in_addr_t ip) {
//   m_addr_ip = ip;
// }
//

