#include "SessionManager.hpp"
#include <cstdlib>

SessionManager::SessionManager()
{
}

SessionManager::SessionManager(const SessionManager& other)
	: _sessions(other._sessions)
{
}

SessionManager& SessionManager::operator=(const SessionManager& other)
{
	if (this != &other)
		_sessions = other._sessions;
	return *this;
}

SessionManager::~SessionManager()
{
}

std::string SessionManager::generateSessionId()
{
	std::string id = "s-";
	int i = 0;

	while (i < 9)
	{
		id += '0' + std::rand() % 10;
		++i;
	}
	return id;
}

std::string SessionManager::createSession()
{
	std::string id = generateSessionId();

	while (_sessions.find(id) != _sessions.end())
		id = generateSessionId();

	_sessions.insert(std::make_pair(id, Session(id)));

	return id;
}

Session* SessionManager::findSession(const std::string& sessionId)
{
	std::map<std::string, Session>::iterator it;

	it = _sessions.find(sessionId);
	if (it == _sessions.end())
		return NULL;

	return &it->second;
}

void SessionManager::removeSession(const std::string& sessionId)
{
	_sessions.erase(sessionId);
}