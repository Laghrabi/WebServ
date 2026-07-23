#include "CgiHandler.hpp"
#include "sys/wait.h"
#include <cstdio>


CgiHandler::CgiHandler(const CgiRequest& cgiRequest) : 
	m_cgi_request(cgiRequest){
}

CgiHandler::CgiHandler(const CgiHandler& other) : 
	m_cgi_request(other.m_cgi_request){
}

void CgiHandler::execute(void) {
	m_cgi_script = "/home/hsacr/COMMON_CORE/webserver/tests/cgi/apache-cgi/cgi-bin/env";
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
		CString p = CString("/home/hsacr/COMMON_CORE/webserver/tests/cgi/apache-cgi/cgi-bin/env");
		std::string l = ("hey");
		char *const var[] = {&l[0], NULL};
		int sucess = execve(m_cgi_script.c_str(), var, m_cgi_request.getEnvp());
		(void)sucess;
		perror(("hey"));
		std::cout << "sucess\n";
	}
	else {
		wait(NULL);
		close(m_pipe_fds[1]);
		char buff[2] = {0};
		ssize_t read_bytes;
		while ((read_bytes = read(m_pipe_fds[0], buff, 1)) > 0) {
			// std::cout << "read bytes = " << read_bytes << "\n";
				std::cout << buff;
			}
		close(m_pipe_fds[0]);
	}
}

CgiHandler& CgiHandler::operator=(const CgiHandler& other) {
	m_cgi_request = other.m_cgi_request;
  return (*this);
}

CgiHandler::~CgiHandler(void) {
}
