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
		void setHttpEnvs(void);
		std::string toCgiEnvName(const std::string& name);
	public:
		CgiRequest(const HttpRequest& request);
		CgiRequest(const CgiRequest& other);
		const CgiRequest& operator=(const CgiRequest& other);
		void printEnv() const;
		char** getEnvp() const;
		const HttpRequest& getHttpRequest(void) const;
		~CgiRequest();

};
