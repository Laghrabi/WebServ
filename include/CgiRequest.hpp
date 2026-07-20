#include "webserver.hpp"
#include "HttpRequest.hpp"
#include "CString.hpp"

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
		void setClientEnv();
		void setEnv();
		void setEnvp(void);
		void detectEnterp(void);
	public:
		CgiRequest(const HttpRequest& request);
		void printEnv() const;
		char** getEnvp() const;
		~CgiRequest();

};
