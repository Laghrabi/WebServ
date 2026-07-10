#include "webserver.hpp"

CgiRequest::CgiRequest(const HttpRequest& request) : m_request(request),
	m_env(NULL){
		setEnv();
	}


void CgiRequest::setRequestEnv() {
	m_env_vec.push_back("REQUEST_URI=" + m_request.getUri());
	m_env_vec.push_back("REQUEST_METHOD=" + m_request.getMethod());
	m_env_vec.push_back("QUERY_STRING=" + m_request.getUri());
	if (m_request.getHeader("Content-Length") != "")
	{

	}
}

void CgiRequest::setServerEnv() {
	m_env_vec.push_back("SERVER_NAME=");
	m_env_vec.push_back(CString("SERVER_SOFTWARE=1337 bengurir webserver"));
	m_env_vec.push_back("SERVER_PROTOCOL=HTTP/1.1");
	m_env_vec.push_back("SERVER_ADDR=localhost");
	m_env_vec.push_back("SERVER_PORT=1000");
}

void CgiRequest::setScriptInfoEnv() {
	m_env_vec.push_back("SCRIPT_FILENAME=");
	m_env_vec.push_back("SCRIPT_NAME=");
}

void CgiRequest::setEnv(void) {
	setRequestEnv();
	setServerEnv();
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


CgiRequest::~CgiRequest() {
	if (m_env)
		delete[] m_env;
}
