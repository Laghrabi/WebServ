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

#include <sys/stat.h>

#include "tokenization.hpp"
#include "findElem.hpp"

#include <fcntl.h>
#include <unistd.h>

typedef std::vector<token> TokenCont;


#include "UnorderedMultiMap.hpp"

#include "utils.tpp"
#include "./MimeTypesExt.hpp"
#include "Server.hpp"
#include "Config.hpp"
#include "ParseConfig.hpp"
#include "Location.hpp"
#include "IPort.hpp"
#include "IPortV4.hpp"
#include "IPortV6.hpp"
#include "ResourceLocator.hpp"
#include "RouteManager.hpp"
#include "HttpRequest.hpp"
#include "StringUtils.hpp"

#define DEFAULT_ADDR 0
#define DEFAULT_PORT 80

#endif
