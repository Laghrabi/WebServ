#include "CgiHandler.hpp"
#include "HttpRequest.hpp"
#include "findElem.hpp"
#include "sys/wait.h"
#include <cstdio>
#include <stdexcept>
#include <utility>


CgiHandler::CgiHandler(const HttpRequest& request) :
m_reading_body(false),
	m_request(request)
	{
	}

CgiHandler::CgiHandler(const CgiHandler& other) : 
m_reading_body(other.m_reading_body),
	m_request(other.m_request)
{
	}

int CgiHandler::execute(void) {
	std::cerr << "executing scrip\n";
	CgiRequest cgi_request(m_request);
	m_cgi_script = m_request._routeResult.targetPath;
	pipe(m_pipe_fds);
	int pid = fork();
	if (pid != -1) {
		// internel server error
	}
	if (pid == 0) { // child
		close (m_pipe_fds[0]);
		int fd = open ("file", O_RDONLY);
		if (fd == -1) {

		}
		if (dup2(fd, 0) )
		{
			;
		}
		close (fd);
		if (dup2(m_pipe_fds[1], 1)) {

		}
		close (m_pipe_fds[1]);
		std::string l = ("hey");
		char *const var[] = {&l[0], NULL};
		std::cerr << "execution valid\n";
		int sucess = execve(m_cgi_script.c_str(), var, cgi_request.getEnvp());
		(void)sucess;
		std::cerr << "execution failed\n";
		// response.makeerror(501);
	}
	std::cerr << "file des is " << m_pipe_fds[0] << "\n";
	close(m_pipe_fds[1]);
	return (m_pipe_fds[0]);
}

// rules
//      CGI-field       = Content-Type | Location | Status must not appear more than once
//      must be at least one
//      content-type:

std::pair<std::string, std::string> CgiHandler::parse_header(const std::string& data) {
	std::size_t colon_pos = data.find(':');

	if (colon_pos == std::string::npos) {
		throw (std::runtime_error("no colon"));
	}

	std::string field_name = data.substr(0, colon_pos);
	std::string field_value = data.substr(colon_pos + 1);

	if (field_name.empty()) throw (std::runtime_error(""));

	std::size_t v = field_value.find_first_not_of(" ");
	if (v == std::string::npos) {
		throw (std::runtime_error("v is nps"));
	}

	return (std::make_pair(field_name, field_value.substr(v)));
}


void CgiHandler::parse(const std::vector<char>& data) {;
	m_data.insert(m_data.end(), data.begin(), data.end());
	// if (!m_reading_body) {
		while (true){
			sleep(1);
			std::vector<char>::iterator nl = std::find(m_data.begin(), m_data.end(), '\n');
			if (nl != m_data.end()) {
				std::string line = std::string(m_data.begin(), nl);
				std::cerr << "line: " << line;
				if (line == "\n") {
					m_reading_body = true;
					break ;
				}
				try {
					std::cout << "header = " << line << "\n";
					std::pair<std::string, std::string> l = parse_header(line);
					std::cout << "name : " << l.first << std::endl;
					std::cout << "value : " << l.second << std::endl;
					m_data.erase(m_data.begin(), nl + 1);
				}
				catch (const std::runtime_error& e) {
					std::cout << "errr\n" << "\n";
					std::cout << e.what() << "\n";
				}
			}
			else {
				// something here
			}
		}
	// }
	// if (m_reading_body) {
	// 		std::cout << "data\n";
	// }
}

CgiHandler& CgiHandler::operator=(const CgiHandler& other) {
	(void)(other);
	return (*this);
}

CgiHandler::~CgiHandler(void) {
}
