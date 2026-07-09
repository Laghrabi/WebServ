#include "webserver.hpp"

class CgiRequest {
	void setRequestEnv(const HttpRequest& request);
	void setEnv();
};
