#include "CgiHandler.hpp"
#include "HttpRequest.hpp"
#include "findElem.hpp"
#include "sys/wait.h"
#include <cstdio>
#include <sstream>
#include <stdexcept>
#include <utility>


CgiHandler::CgiHandler(const HttpRequest& request, HttpResponse& response) :
	m_reading_body(false),
	m_request(request),
	m_response(response),
	m_send_buffer(response.buffer)
{
}

CgiHandler::CgiHandler(const CgiHandler& other) : 
	m_reading_body(other.m_reading_body),
	m_request(other.m_request),
	m_response(other.m_response),
	m_send_buffer(other.m_send_buffer)
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

	std::size_t field_value_pos = field_value.find_first_not_of(" \t\r");
	if (field_value_pos == std::string::npos) {
		throw (std::runtime_error("no value field_value" + field_value));
	}

	return (std::make_pair(field_name, field_value.substr(field_value_pos)));
}

void checkForError(std::string& header,const std::string& value) {
	(void)value;
	if (!header.empty()) {
		throw (std::runtime_error("internel server error"));
		// make error 500
	}
}

bool CgiHandler::isCgiField(const std::string& field_name, const std::string& field_value) {
	std::cout << "field_value " << field_value << "\n";
	if (field_name == "Location" || field_name == "Status" ||
			field_name == "Status") {
		if (field_name == "Location")	 {
			m_location = field_value;
		}
		if (field_name == "Status")	 {
			int val;
			std::string str;
			std::stringstream ss(field_value);
			if ((ss >> val) && (ss >> str)) {
				m_location = field_value;
			}
			else {
				throw (std::runtime_error("status error"));
			}
			m_status = field_value;
		}
		if (field_name == "Status") {
			m_location = field_value;
		}
		return (true);
	}
	return (false);
}

template <typename T> void appendStringToVec(T& c, typename T::iterator it, const std::string& str) {
	c.insert(it, str.begin(), str.end());
}

// status void
void CgiHandler::checkHeader(const std::string& header) {
	std::pair<std::string, std::string> pair = parse_header(header);	
	std::string &field_name = pair.first;
	std::string &field_value = pair.second;

	bool is_cgi_field = isCgiField(field_name, field_value);
	if (!is_cgi_field || (is_cgi_field && field_name != "Status")) {
		std::cout << "[CGI] insert a new header: " << field_name << "\n";
		VecIter end = m_send_buffer.end();
		m_send_buffer.insert(end, header.begin(), header.end());
		std::cout << "[CGI] appending \\r\\n to the header to put in in buffer send\n";
		appendStringToVec(m_send_buffer, m_send_buffer.end(), "\r\n");
	}
}

std::string toHex(std::size_t num) {
	std::stringstream ss;
	ss << std::hex << num;
	return (ss.str());
}

template<typename T> void appendCRLF(T& c, typename  T::iterator it) {
	appendStringToVec(c, it, "\r\n");
}

void CgiHandler::setChunckedBody() {
	std::size_t chunck_size = m_data.size();
	std::cout << "[CGI] body chunck size " << chunck_size << "\n";
	// exit (20);
	std::string chunck_hex = toHex(chunck_size);
	appendStringToVec(m_send_buffer, m_send_buffer.end(), chunck_hex + "\r\n");
	m_send_buffer.insert(m_send_buffer.end(), m_data.begin(), m_data.end());
	appendCRLF(m_send_buffer, m_send_buffer.end());
}

void CgiHandler::parseBody() {
	if (m_state == STORE_BODY) {
		setChunckedBody();
		std::cout << m_data.size() << "[CGI] saving body\n importnat data[";
		// write(1, &data[0], data.size());
		std::cout << "]";

	}
}

void CgiHandler::addEssentialHeaders() {
	appendStringToVec(m_send_buffer, m_send_buffer.end(),
			"Server: 1337-webserver\r\n");
	// appendStringToVec(m_send_buffer, m_send_buffer.end(),
	// 		"\r\n");
}



void CgiHandler::setBodyState() {
	std::cout << "[CGI] STORE BODY NORMALLY\n";
	m_state = STORE_BODY;
	if (!m_status.empty()) {
		std::cout << "[CGI] status: " << m_status << "\n";
		m_status += "\r\n";
		m_send_buffer.insert(m_send_buffer.begin(), m_status.begin(), m_status.end());
		appendStringToVec(m_send_buffer, m_send_buffer.begin(), "HTTP/1.1 ");
	}
	else if (!m_location.empty()) {
		m_state = BODY_NOT_USEFUL;
	}
	else {
		appendStringToVec(m_send_buffer, m_send_buffer.begin(), "HTTP/1.1 200 OK\r\n");
	}
	appendStringToVec(m_send_buffer, m_send_buffer.end(), "\r\n");
}


void CgiHandler::parse(const std::vector<char>& data) {
	m_data.insert(m_data.end(), data.begin(), data.end());
	if (!m_reading_body) {
		while (true){
			std::vector<char>::iterator nl = std::find(m_data.begin(), m_data.end(), '\n');
			if (nl != m_data.end()) {
				std::string line = std::string(m_data.begin(), nl);
				if (line == "") {
					// m_data.clear();
					m_data.erase(m_data.begin(), nl + 1);
					setBodyState();
					std::cerr << "[CGI] reading body" << "\n";
					m_response.setHeadersSent(true);
					m_reading_body = true;
					break ;
				}
				try {
					checkHeader(line);
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
		parseBody();
		m_data.clear();
	}
}

CgiHandler& CgiHandler::operator=(const CgiHandler& other) {
	(void)(other);
	return (*this);
}

CgiHandler::~CgiHandler(void) {
}
