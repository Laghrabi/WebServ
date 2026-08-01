#include "CgiHandler.hpp"
#include "HttpRequest.hpp"
#include "sys/wait.h"
#include <cstdio>


CgiHandler::CgiHandler(const HttpRequest& request) :
m_request(request){
}

CgiHandler::CgiHandler(const CgiHandler& other) : 
	m_request(other.m_request){
}

int CgiHandler::execute(void) {
	std::cerr << "executing scrip\n";
	CgiRequest cgi_request(m_request);
	m_cgi_script = "/home/hsacr/COMMON_CORE/webserver/tests/cgi/apache-cgi/cgi-bin/a.out";
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
		CString p = CString("/home/hsacr/COMMON_CORE/webserver/tests/cgi/apache-cgi/cgi-bin/a.out");
		std::string l = ("hey");
		char *const var[] = {&l[0], NULL};
		std::cerr << "execution valid\n";
		int sucess = execve(m_cgi_script.c_str(), var, cgi_request.getEnvp());
		std::cerr << "execution failed\n";
		(void)sucess;
		// perror(("hey"));
		// std::cout << "sucess\n";
	}
	std::cerr << "file des is " << m_pipe_fds[0] << "\n";
		close(m_pipe_fds[1]);
	// else {
	// 	wait(NULL);
	// 	char buff[2] = {0};
	// 	ssize_t read_bytes;
	// 	while ((read_bytes = read(m_pipe_fds[0], buff, 1)) > 0) {
	// 		// std::cout << "read bytes = " << read_bytes << "\n";
	// 			std::cerr << buff << std::endl;
	// 	}
	// 	close(m_pipe_fds[0]);
	// }
	return (m_pipe_fds[0]);
}

CgiHandler& CgiHandler::operator=(const CgiHandler& other) {
	(void)(other);
  return (*this);
}

CgiHandler::~CgiHandler(void) {
}
