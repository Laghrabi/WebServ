#include "Session.hpp"

Session::Session()
	: _sessionId(""), _visitCount(0)
{
}

Session::Session(const std::string& sessionId)
	: _sessionId(sessionId), _visitCount(0)
{
}

Session::Session(const Session& other)
	: _sessionId(other._sessionId), _visitCount(other._visitCount)
{
}

Session& Session::operator=(const Session& other)
{
	if (this != &other)
	{
		_sessionId = other._sessionId;
		_visitCount = other._visitCount;
	}
	return *this;
}

Session::~Session()
{
}

const std::string& Session::getSessionId() const
{
	return _sessionId;
}

unsigned int Session::getVisitCount() const
{
	return _visitCount;
}

void Session::visit()
{
	++_visitCount;
}