#ifndef _WEBSERVER_H
#define _WEBSERVER_H

/*          stl containers           */

#include <cstring>
#include <cerrno>

#include <list>
#include <map>
#include <vector>
#include <set>
#include <deque>

#include <algorithm>

#include <iostream>
#include <fstream>
#include <iomanip>

#include <stdlib.h>
#include <stdio.h>


#include <netinet/in.h>
#include <arpa/inet.h>

#include <exception>
#include <sstream>
#include <string>
#include <iterator>

#include <sys/types.h>
#include <sys/socket.h>
# include <sys/stat.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <dirent.h>

#include <sys/stat.h>

#include "tokenization.hpp"
#include "findElem.hpp"

#include <fcntl.h>
#include <unistd.h>

typedef std::vector<token> TokenCont;

#include "HttpStatus.hpp"


#include "UnorderedMultiMap.hpp"

#include "utils.tpp"
#include "./FileStatus.hpp"
#include "./MimeTypesExt.hpp"
#include "RouteConfig.hpp"
#include "RouteNode.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "ParseConfig.hpp"
#include "Location.hpp"
#include "IPort.hpp"
#include "IPortV4.hpp"
#include "IPortV6.hpp"
#include "HttpRequest.hpp"
#include "StringUtils.hpp"
// #include "CgiRequest.hpp"
#include "HttpResponse.hpp"
// #include "HttpRequestHandler.hpp"

std::string to_string(std::size_t num);

void copyArrayToVec(char* first, std::size_t n, std::vector<char>& vec);

#define DEFAULT_ADDR 0
#define DEFAULT_PORT 80

#endif
