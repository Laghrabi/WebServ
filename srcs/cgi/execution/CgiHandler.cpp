#include "CgiHandler.hpp"
#include "HttpStatus.hpp"
#include "HttpStatus.hpp"
#include "webserver.hpp"
#include <cctype>
#include <cstdio>

CgiHandler::CgiHandler(const HttpRequest& request, HttpResponse& response) :
	m_reading_body(false),
	m_request(request),
	m_response(response),
	m_send_buffer(response.buffer),
	m_pid(-1),
	m_ok(true)
{
}

CgiHandler::CgiHandler(const CgiHandler& other) : 
	m_reading_body(other.m_reading_body),
	m_request(other.m_request),
	m_response(other.m_response),
	m_send_buffer(other.m_send_buffer),
	m_pid(other.m_pid),
	m_ok(other.m_ok),
	m_last_read(other.m_last_read),
	m_start_time(other.m_start_time)
{
}

template <typename T> void appendStringToVec(T& c, typename T::iterator it, const std::string& str) {
	c.insert(it, str.begin(), str.end());
}

bool CgiHandler::checkTimeOut() {
	std::size_t current_time = std::time(NULL);
	std::size_t relative_time = current_time - m_last_read;
		std::cout << "[CGI] timeout script " << relative_time << "\n"<< m_cgi_script << "\n";
	if (relative_time > 5) {
		std::cout << "[CGI] timeout script " << relative_time << "\n"<< m_cgi_script << "\n";
		return (true);
	}
	std::size_t absolute_time = current_time - m_start_time;
if (absolute_time > 10) {
		std::cout << "[CGI] timeout script " << relative_time << "\n"<< m_cgi_script << "\n";
		return (true);
	}

	return (false);
}

void CgiHandler::checkProcessState() {
	if (checkTimeOut()) {
		kill (m_pid, SIGKILL);
	}
	int pid = waitForProcess();
	if (pid > 0 && !m_reading_body) {
		std::cout << "[CGI] internel server errror\n";
		m_response.is_ok_send = true;
		m_response.is_finished = true;
		m_response.makeErrorCgi(INTERNAL_SERVER_ERROR, m_request);
	}
	else if (pid > 0 && m_reading_body && m_ok) {
		if (m_state == STORE_BODY)  {
			std::cout << "[CGI] adding \\r\\n0\\r\\n.\n";
			std::cout << m_send_buffer.size() << "\n";
			appendStringToVec(m_send_buffer, m_send_buffer.end(), "0\r\n\r\n");
			std::cout << m_send_buffer.size() << "\n";
		}
		if (m_status.empty() && m_location.empty())
			m_response.last_code = OK;
		m_response.is_ok_send = true;
		m_response.is_finished = true;
	}
	else if (pid == -1) {
		m_response.is_ok_send = true;
		m_response.is_finished = true;
	}
}

int CgiHandler::waitForProcess() {
	int status = 0;
	int pid;
	if ((pid = waitpid(m_pid, &status, WNOHANG)) == 0) {
		// std::cout << "[CGI] no change in state\n";
	}
	else if (pid > 0) {
		std::cout << "[CGI] process terminate " << m_cgi_script << "\n";
	}
	return (pid);
}

void CgiHandler::killProcess() {
	if (m_pid != -1) {
		kill (m_pid, SIGKILL);
		waitForProcess();
	}
}

void CgiHandler::handleChild() {
	CgiRequest cgi_request(m_request);
	std::string fileIn = m_request.getBodyFilePath().c_str();
	int fd = -1;
	if (!fileIn.empty())
	{
		if ((fd= open(fileIn.c_str(), O_RDONLY))) {
			;
		}
		if (dup2(fd, 0))
		{
			close (fd);
			//NOTE: internel server errror
			std::cerr << "[CGI] fail to dup file to 0 " << m_request.getBodyFilePath().c_str() << "\n";
		}
		else
			close (fd);
	}
	std::string wdir = m_cgi_script.substr(0, m_cgi_script.find_last_of('/'));
	if (wdir.empty()) wdir = "/";
	std::cout << "[CGI] setting working dir to " << wdir << std::endl;
	chdir(wdir.c_str());
	// if (fd == -1) {
	// 	std::cerr << "[CGI] can't open file " << m_request.getBodyFilePath().c_str() << "\n";
	// }

	if (dup2(m_pipe_fds[1], 1))
	{
		// TODO: do something here exit is not an option maybe
	}

	close (m_pipe_fds[1]);
	CString l(m_cgi_script);
	char *const var[] = {l.getCstr(), NULL};
	int sucess = execve(m_cgi_script.c_str(), var, cgi_request.getEnvp());
	(void)sucess;
	std::cerr << "[CGI] execution for script " << m_cgi_script << " failed\n";
}

int CgiHandler::execute(void) {
	std::cerr << "[CGI] start setup executing scrip\n";
	m_cgi_script = m_request._routeResult.targetPath;
	int fail = pipe(m_pipe_fds);
	if (fail)
	{
	}
	std::cout << "[CGI] opening file for the script to read " << m_request.getBodyFilePath().c_str() << std::endl;

	m_pid = fork();
	if (m_pid != -1) {
		// internel server error
	}

	if (m_pid == 0) { // child
		handleChild();
	}
	m_last_read = std::time(NULL);
	m_start_time = m_last_read;
	close(m_pipe_fds[1]);
	return (m_pipe_fds[0]);
}

// rules
//      CGI-field       = Content-Type | Location | Status must not appear more than once
//      must be at least one
//      content-type:

std::pair<std::string, std::string> CgiHandler::parse_header(const std::string& data) {
	std::cout << "[CGI] " << data << "\n";
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
	m_response.last_code = (HttpStatus)val;
	std::cout << "[CGI] set code to " << val << "\n";
	m_status = field_value;
}


bool CgiHandler::isCgiField(const std::string& field_name, const std::string& field_value) {
	if (compare_header(field_name, "location") || 
			compare_header(field_name, "status") ||
			compare_header(field_name, "content-type")) {
		if (compare_header(field_name, "location"))	 {
			if (!m_location.empty())
				throw (std::runtime_error("got location two times\n"));
			m_location = field_value;
		}
		if (compare_header(field_name, "Content-type"))	 {
			if (!m_content_type.empty())
				throw (std::runtime_error("got Content Type two times\n"));
			m_content_type = field_value;
		}
		if (compare_header(field_name, "status"))	 {
			if (!m_status.empty())
				throw (std::runtime_error("got status two times\n"));
			parseStatus(field_value);
		}
		return (true);
	}
	return (false);
}

bool CgiHandler::isHeaderEgnored(const std::string& field_name) {
	if (compare_header(field_name, "connection") ||
			compare_header(field_name, "date") ||
			compare_header(field_name, "server") ||
			compare_header(field_name, "content-length") ||
			compare_header(field_name, "transfer-encoding"))
			{
		return (true);
	}
	return (false);
}

void CgiHandler::addHeader(const std::string& header) {
	appendToSendBuffer(m_send_buffer.end(), header + "\r\n")	;
}

// status void
void CgiHandler::checkHeader(const std::string& header) {
	std::pair<std::string, std::string> pair = parse_header(header);	
	std::string &field_name = pair.first;
	std::string &field_value = pair.second;

	if (isHeaderEgnored(field_name)) {
		return ;
	}

	bool is_cgi_field = isCgiField(field_name, field_value);
	if (!is_cgi_field || (is_cgi_field && field_name != "Status")) {
		std::cout << "[CGI] insert a new header: [" << field_name << "]\n";
		VecIter end = m_send_buffer.end();
		m_send_buffer.insert(end, header.begin(), header.end());
		// std::cout << "[CGI] appending \\r\\n to the header to put in in buffer send\n";
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
	std::string chunck_hex = toHex(chunck_size);
	appendStringToVec(m_send_buffer, m_send_buffer.end(), chunck_hex + "\r\n");
	m_send_buffer.insert(m_send_buffer.end(), m_data.begin(), m_data.end());
	std::cout << "[CGI] insert data\n";
	appendCRLF(m_send_buffer, m_send_buffer.end());
}

void CgiHandler::parseBody() {
	if (m_state == STORE_BODY) {
		setChunckedBody();
		std::cout << "[CGI] saving body to Send Buffer\n";
	}
}

void CgiHandler::addEssentialHeaders() {
	appendStringToVec(m_send_buffer, m_send_buffer.end(),
			"Server: 1337-webserver\r\n");
	appendStringToVec(m_send_buffer, m_send_buffer.end(),
			"Date: " + HttpResponse::getCurrentDate() + "\r\n");
	if (compare_header(m_request.getHeader("connection"), "close")) {
		addHeader("Connection: close");
		m_response.keep_connection = false;
	}
	else{
		addHeader("Connection: keep-alive");
		m_response.keep_connection = true;
	}
}


void CgiHandler::appendToSendBuffer(std::vector<char>::iterator it, const std::string& str) {
	appendStringToVec(m_send_buffer, it, str);
}

void CgiHandler::handleLocation() {
	appendStringToVec(m_send_buffer, m_send_buffer.begin(), "HTTP/1.1 302 Found \r\n");
	std::string body = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">"
		"<html><head>"
		"<title>302 Found</title>"
		"</head><body>"
		"<h1>Found</h1>"
		"<p>The document has moved <a href=\"" + std::string(m_location) + "\">here</a>.</p>"
		"<hr>"
		"</body></html>";
	addHeader("Content-Lenght: " + to_string(body.size()));
	addHeader("Content-Lenght: text/html");
	appendCRLF(m_send_buffer, m_send_buffer.end());
	appendToSendBuffer(m_send_buffer.end(), body);
	m_response.last_code = FOUND;
}

void CgiHandler::setBodyState() {
	std::cout << "[CGI] STORE BODY NORMALLY\n";
	if (m_location.empty() && m_status.empty()
			&& m_content_type.empty()) {
		std::cout << "[CGI] ERROR [cgi fileds are not set]\n";
		killProcess();
		m_send_buffer.clear();
		m_response.makeErrorCgi(INTERNAL_SERVER_ERROR, m_request);
		m_response.is_finished = true;
		m_response.is_ok_send = true;
		m_ok = false;
		return ;
	}
	m_state = STORE_BODY;
	addEssentialHeaders();
	if (!m_status.empty()) {
		std::cout << "[CGI] status: " << m_status << "\n";
		m_status += "\r\n";
		m_send_buffer.insert(m_send_buffer.begin(), m_status.begin(), m_status.end());
		appendStringToVec(m_send_buffer, m_send_buffer.begin(), "HTTP/1.1 ");
	}
	else if (!m_location.empty()) {
		handleLocation();
		// appendStringToVec(m_send_buffer, m_send_buffer, );
		m_state = BODY_NOT_USEFUL;
	}
	else {
		appendStringToVec(m_send_buffer, m_send_buffer.begin(), "HTTP/1.1 200 OK\r\n");
	}
	if (m_state == STORE_BODY) {
		appendStringToVec(m_send_buffer, m_send_buffer.end(), "Transfer-Encoding: chunked\r\n");
	}
	appendStringToVec(m_send_buffer, m_send_buffer.end(), "\r\n");
}


void CgiHandler::parse(const std::vector<char>& data) {
	m_last_read = std::time(NULL);
	if (!m_ok)
		return ;
	std::cout << "=================================my data\n";
	std::cout << waitForProcess() << "|n\n";
	write (1, &data[0], data.size());
	std::cout << "=================================my data\n";
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
					m_response.is_ok_send = true;
					if (m_ok)
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
					m_response.makeErrorCgi(INTERNAL_SERVER_ERROR, m_request);
					killProcess();
					m_response.is_finished = true;
					m_ok = false;
					// NOTE: here internel server errror
					// terminate the script
					std::cout << "[CGI] "<< e.what() << "\n";
					break ;
				}
			}
			else {
				break ;
			}
		}
	}
	if (m_reading_body && !m_data.empty()) {
		parseBody();
		m_data.clear();
	}
}

CgiHandler& CgiHandler::operator=(const CgiHandler& other) {
if (this != &other)
    {
        m_state = other.m_state;
        m_headers = other.m_headers;
        // m_bodyBytes = other.m_bodyBytes;
        m_status = other.m_status;
        m_location = other.m_location;
        m_content_type = other.m_content_type;
        m_reading_body = other.m_reading_body;
        m_data = other.m_data;
        m_cgi_script = other.m_cgi_script;

        m_pipe_fds[0] = other.m_pipe_fds[0];
        m_pipe_fds[1] = other.m_pipe_fds[1];

        m_pid = other.m_pid;
        m_ok = other.m_ok;
        m_last_read = other.m_last_read;
				m_start_time = other.m_start_time;
    }

	return (*this);
}

CgiHandler::~CgiHandler(void) {
}
