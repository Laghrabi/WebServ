#include "CgiHandler.hpp"
#include "HttpRequest.hpp"
#include "findElem.hpp"
#include "sys/wait.h"
#include <cstdio>
#include <stdexcept>
#include <utility>


CgiHandler::CgiHandler(const HttpRequest& request, HttpResponse& response) :
	m_reading_body(false),
	m_request(request),
	m_response(response)
{
}

CgiHandler::CgiHandler(const CgiHandler& other) : 
	m_reading_body(other.m_reading_body),
	m_request(other.m_request),
	m_response(other.m_response)
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

void checkForError(std::string& header,const std::string& value) {
	(void)value;
	if (!header.empty()) {
		throw (std::runtime_error("internel server error"));
		// make error 500
	}
}

// status void
void CgiHandler::checkCgiHeader(std::pair<std::string, std::string> header_field) {
	if (header_field.first == "Location")	 {
		m_location = header_field.second;
	}
	if (header_field.first == "Status")	 {
		m_location = header_field.second;
	}
	if (header_field.first == "Status")	 {
		m_location = header_field.second;
	}
}


void CgiHandler::parseBody(const std::vector<char>& data) {
	if (m_state == STORE_BODY) {
		m_response.buffer.insert(m_response.buffer.end(), data.begin(), data.end());
	}
}


void CgiHandler::setBodyCase() {
	std::cout << "[CGI] STORE BODY NORMALLY\n";
	m_state = STORE_BODY;
	if (!m_status.empty()) {
		std::string start_line = "200 something hey";
		// add headers here // NOTE: it is best to add them before
	}
	else if (!m_location.empty()) {
		m_state = BODY_NOT_USEFUL;
	}
}


void CgiHandler::parse(const std::vector<char>& data) {
	if (!m_reading_body) {
	m_data.insert(m_data.end(), data.begin(), data.end());
		while (true){
			// sleep(1);
			std::vector<char>::iterator nl = std::find(m_data.begin(), m_data.end(), '\n');
			if (nl != m_data.end()) {
				std::string line = std::string(m_data.begin(), nl);
				if (line == "") {
					setBodyCase();
					// m_response.setHeadersSent(true);
					std::cerr << "[CGI] " << &m_response.buffer[0] << "\n";
					std::cerr << "[CGI] " << m_response.buffer.size() << "\n";
					m_reading_body = true;
					break ;
				}
				try {
					std::pair<std::string, std::string> header = parse_header(line);
					checkCgiHeader(header);
					std::vector<char>::iterator end = m_response.buffer.end();
					m_response.buffer.insert(end, m_data.begin(), nl);
					m_data.erase(m_data.begin(), nl + 1);
				}
				catch (const std::runtime_error& e) {
					// NOTE: here internel server errror
					// terminate the script
					std::cout << e.what() << "\n";
				}
			}
			else {
				break ;
			}
		}
	}
	if (m_reading_body) {
		parseBody(data);
		// std::cout << "[CGI_DEBUG]" << "\n";
		// 	std::cout << "BODY [" << std::string(m_data.begin(), m_data.end()) << "]\n";
	}
}

CgiHandler& CgiHandler::operator=(const CgiHandler& other) {
	(void)(other);
	return (*this);
}

CgiHandler::~CgiHandler(void) {
}
