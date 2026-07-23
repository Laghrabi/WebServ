#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP
#include <iostream>


#include "CgiRequest.hpp"

class CgiHandler {
	private:
	CgiRequest m_cgi_request;
	std::string m_cgi_script;
	int m_pipe_fds[2];
  public:
    CgiHandler(const CgiRequest& cgiRequest);
    CgiHandler(const CgiHandler& other);
		void execute(void);
    CgiHandler& operator=(const CgiHandler& other);
    ~CgiHandler(void);
};

#endif
