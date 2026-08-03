#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP
#include <iostream>
#include <vector>


#include "CgiRequest.hpp"
#include "HttpRequest.hpp"

class CgiHandler {
	private:
	bool m_reading_body;
	std::vector<char> m_data;
	const HttpRequest& m_request;
	std::string m_cgi_script;
	int m_pipe_fds[2];
  CgiHandler& operator=(const CgiHandler& other);
  public:
    CgiHandler(const HttpRequest& cgiRequest);
    CgiHandler(const CgiHandler& other);
		int execute(void);
		void parse(const std::vector<char>& data);
		std::pair<std::string, std::string> parse_header(const std::string& data);
    ~CgiHandler(void);
};

#endif
