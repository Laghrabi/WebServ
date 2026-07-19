#include "webserver.hpp"
#include "HttpRequest.hpp"

class CString {
	private:
		char *m_ptr;
		std::size_t m_size;
	public:

		CString() : 
			m_ptr(NULL),
			m_size(0){}

		CString(const std::string& str)	 {
			dupString(str.c_str(), str.length());
		}

		void dupString(const char* str, std::size_t len) {
			m_size = len;
			m_ptr = new char[m_size + 1];
			m_ptr[m_size] = '\0';
			std::strcpy(m_ptr, str);
		}

		CString(const char* str)	 {
			dupString(str, std::strlen(str));
		}


		CString(const CString& other) {
			dupString(other.m_ptr, other.m_size);
		}

		CString&	operator=(const CString& other) {
			delete[] m_ptr;
			dupString(other.m_ptr, other.m_size);
			return (*this);
		}

		char* getCstr() const{
			return (m_ptr);
		}

		~CString() {
			delete[] m_ptr;
		}
};

class CgiRequest {
	private:
		typedef CString dataType;
		typedef std::vector<dataType> Cont;
		typedef std::vector<dataType>::iterator ContIter;
		typedef std::vector<dataType>::const_iterator ContConstIter;
		HttpRequest m_request;
		std::vector<CString> m_env_vec;
		char **m_env;
		void setRequestEnv();
		void setServerEnv();
		void setScriptInfoEnv();
		void setEnv();
		void setEnvp(void);
		void detectEnterp(void);
	public:
		CgiRequest(const HttpRequest& request);
		void printEnv() const;
		char** getEnvp() const;
		~CgiRequest();

};
