#include "CgiRequest.hpp"

/* NOTE: 
 * GATEWAY_INTERFACE
 * PATH_INFO                             FOUND
 * QUERY_STRING [EXIST_ALWAYS]            FOUND
 * REMOTE_ADDR [EXIST_ALWAYS]           FOUND
 * REMOTE_HOST [not exist]             FOUND
 * REMOTE_IDENT [not must]
 * REMOTE_USER [ idk ]
 * REQUEST_METHOD                         FOUND
 * SCRIPT_NAME  [EXIST_ALWAYS] FOUND
 * SERVER_NAME = hostname | ipv4-address | ( "[" ipv6-address "]"
 * SERVER_PORT [EXIST ALWAYS] FOUND
 * SERVER_SOFTWARE [EXIST ALWAYS] FOUND
 * */

std::string CgiRequest::toCgiEnvName(const std::string& name) {
	std::string res = "HTTP_" + name;
	std::size_t pos = 0;
	while (pos != res.length()) {
		if (res.at(pos) == '-') {
			res[pos] = '_';
		}
		res[pos] = std::toupper(res[pos]);
	}
	return (res);
}

CgiRequest::CgiRequest(const HttpRequest& request) : m_request(request),
	m_env(NULL){
		setEnv();
	}


CgiRequest::CgiRequest(const CgiRequest& other) : 
m_request(other.m_request),
	m_env_vec(other.m_env_vec){
		m_env = new char*[m_env_vec.size() + 1];
	int i = 0;
	for (ContConstIter it = m_env_vec.begin(); it != m_env_vec.end(); ++it, ++i) {
		m_env[i] = it->getCstr();
	}
	m_env[i] = NULL;
}


 const CgiRequest& CgiRequest::operator=(const CgiRequest& other)
	{
		m_env_vec.clear();
		delete[] m_env;
		m_request = other.m_request;
		m_env_vec = other.m_env_vec;
		m_env = new char*[m_env_vec.size() + 1];
	int i = 0;
	for (ContConstIter it = m_env_vec.begin(); it != m_env_vec.end(); ++it, ++i) {
		m_env[i] = it->getCstr();
	}
	m_env[i] = NULL;
	return (*this);
}


void CgiRequest::setRequestEnv() {
	m_env_vec.push_back("REQUEST_URI=" + m_request.getEncodedUri());
	m_env_vec.push_back("REQUEST_METHOD=" + m_request.getMethod());
	m_env_vec.push_back("QUERY_STRING=" + m_request.getQueryString());
	m_env_vec.push_back("PATH_INFO=" + m_request._routeResult.cgiInfo.pathInfo);
	if (m_request.getHeader("Content-Length") != "")
	{

	}
}

void CgiRequest::setClientEnv() {

	m_env_vec.push_back("REMOTE_ADDR=" + m_request.getClientIPort().getIpStr());
	m_env_vec.push_back("REMOTE_PORT=" + m_request.getClientIPort().getIpStr());

	// m_env_vec.push_back("REMOTE_USER=" + m_request.getClientIPort().getIpStr());
	
}


void CgiRequest::setHttpEnvs(void) {
	const std::map<std::string, std::string>& headers_map =  m_request.getHeaders();
	std::map<std::string, std::string>::const_iterator it = headers_map.begin();
	for ( ; it != headers_map.end(); ++it) {
		std::string http_env = toCgiEnvName(it->first);
		m_env_vec.push_back(http_env + "=" + it->second);
	}
}

void CgiRequest::setServerEnv() {
	// server name will be always, and if server_name directive doesn't exist it will be empty string SERVER_NAME=
	m_env_vec.push_back("SERVER_NAME="); // here put server name
	m_env_vec.push_back(CString("SERVER_SOFTWARE=1337 bengurir webserver"));
	const Server::IPort& iport = m_request.getServerIPort();


// NOTE:   Note that this variable MUST be set, even if the port is the default
 //  port for the scheme and could otherwise be omitted from a URI.
	m_env_vec.push_back("SERVER_ADDR=" + iport.getIpStr());
	m_env_vec.push_back("SERVER_NAME="); // INFO: i nee that one
	m_env_vec.push_back("SERVER_PORT=" + m_request.getServerIPort().getPortStr());
	m_env_vec.push_back("SERVER_PROTOCOL=HTTP/1.1");
}

void CgiRequest::setScriptInfoEnv() {
	m_env_vec.push_back("SCRIPT_FILENAME=");
	m_env_vec.push_back("SCRIPT_NAME=" + m_request._routeResult.cgiInfo.scriptName);
}

void CgiRequest::set() {
	m_env_vec.push_back("GATEWAY_INTERFACE=CGI/1.1");

}

void CgiRequest::setEnv(void) {
	setRequestEnv();
	setServerEnv();
	setClientEnv();
	setScriptInfoEnv();
	m_env = new char*[m_env_vec.size() + 1];
	int i = 0;
	for (ContConstIter it = m_env_vec.begin(); it != m_env_vec.end(); ++it, ++i) {
		m_env[i] = it->getCstr();
	}
	m_env[i] = NULL;
}

char** CgiRequest::getEnvp() const {
	return (m_env);
}

void CgiRequest::printEnv() const{
	char **env = m_env;
	while (*env) {
		std::cout << *env << '\n';
		++env;
	}
}


const HttpRequest& CgiRequest::getHttpRequest(void) const {
			return (m_request);
}


CgiRequest::~CgiRequest() {
	if (m_env)
		delete[] m_env;
}
