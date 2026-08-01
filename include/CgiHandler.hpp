#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP
#include <iostream>


#include "CgiRequest.hpp"
#include "HttpRequest.hpp"

class CgiHandler {
	private:
	const HttpRequest& m_request;
	std::string m_cgi_script;
	int m_pipe_fds[2];
  CgiHandler& operator=(const CgiHandler& other);
  public:
    CgiHandler(const HttpRequest& cgiRequest);
    CgiHandler(const CgiHandler& other);
		int execute(void);
    ~CgiHandler(void);
};

#endif
