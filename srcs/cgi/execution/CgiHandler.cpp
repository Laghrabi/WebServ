#include "CgiHandler.hpp"
#include "HttpRequest.hpp"
#include "findElem.hpp"
#include "sys/wait.h"
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <unistd.h>
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

 void CgiHandler::checkProcessState() {
// NOTE: wailt pid
// check time out 
// check stat of the handler: error, finished
// set m_response.is_finished to true
int status;
int success;
	if ((success = waitpid(m_pid, &status, WNOHANG)) == 0) {
		// std::cout << "[CGI] no change in state\n";
		return ;
	}
	else if (status > 0) {
		std::cout << "[CGI] process terminate\n";
		m_response.is_finished = true;
	}
	// else {
	// 	std::cout << "[CGI] No process at All\n";
	// }
 }

void CgiHandler::killProcess() {
	kill (m_pid, SIGKILL);
	checkProcessState();
}

int CgiHandler::execute(void) {
	std::cerr << "[CGI] start setup executing scrip\n";
	CgiRequest cgi_request(m_request);
	m_cgi_script = m_request._routeResult.targetPath;
	pipe(m_pipe_fds);
		std::cout << "[CGI] opening file for the script to read\n" << m_request.getBodyFilePath().c_str() << std::endl;

	m_pid = fork();
	if (m_pid != -1) {
		// internel server error
	}

	if (m_pid == 0) { // child
		close (m_pipe_fds[0]);
		int fd = open (m_request.getBodyFilePath().c_str(), O_RDONLY);
		std::string wdir = m_cgi_script.substr(0, m_cgi_script.find_last_of('/'));
		std::cout << "[CGI] setting working dir to " << wdir << std::endl;
		chdir(wdir.c_str());
		if (fd == -1) {
			std::cerr << "[CGI] can't open file " << m_request.getBodyFilePath().c_str() << "\n";
		}
		if (dup2(fd, 0) )
		{
			std::cerr << "[CGI] fail to dup file to 0 " << m_request.getBodyFilePath().c_str() << "\n";
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
		throw (std::runtime_error("no colon filed: " + data));
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


void CgiHandler::parseStatus(const std::string& field_value) {
		std::string str;
			int val;
			std::stringstream ss(field_value);
			if ((ss >> val) && (ss >> str)) {
				m_location = field_value;
			}
			else {
				throw (std::runtime_error("status error"));
			}
			m_status = field_value;
}

bool CgiHandler::isCgiField(const std::string& field_name, const std::string& field_value) {
	if (field_name == "Location" || field_name == "Status" ||
			field_name == "Content-Type") {
		if (field_name == "Location")	 {
			if (!m_location.empty())
				throw (std::runtime_error("got location two times\n"));
			m_location = field_value;
		}
		if (field_name == "Status")	 {
			if (!m_status.empty())
				throw (std::runtime_error("got status two times\n"));
			parseStatus(field_value);
		}
		if (field_name == "Content-Type") {
			if (!m_content_type.empty())
				throw (std::runtime_error("got Content Type two times\n"));
			m_content_type = field_value;
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
		std::cout << "[CGI] insert a new header: [" << field_name << "]\n";
		VecIter end = m_send_buffer.end();
		m_send_buffer.insert(end, header.begin(), header.end());
		std::cout << "[CGI] appending \\r\\n to the header to put in in buffer send\n";
		appendStringToVec(m_send_buffer, m_send_buffer.end(), "\r\n");
	}
}

static std::string toHex(std::size_t num) {
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
		std::cout << m_data.size() << "[CGI] saving body to Send Buffer\n";
		setChunckedBody();
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
					m_data.erase(m_data.begin(), nl + 1);
					setBodyState();
					std::cerr << "[CGI] reading body phase" << std::endl;
					m_response.setHeadersSent(true);
					m_reading_body = true;
					break ;
				}
				try {
					checkHeader(line);
					m_data.erase(m_data.begin(), nl + 1);
				}
				catch (const std::runtime_error& e) {
					std::cout << "[CGI] malformed header, clearing send buffer and set internel server error\n";
					m_send_buffer.clear();
					// NOTE: here internel server errror
					// terminate the script
					std::cout << "[CGI] "<< e.what() << "\n";
					killProcess();
					break ;
					// exit (20);
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
