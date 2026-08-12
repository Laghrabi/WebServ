#ifndef SESSIONMANAGER_HPP
#define SESSIONMANAGER_HPP

#include "webserver.hpp"

class SessionManager
{
private:
    std::map<std::string, Session> _sessions;

    std::string generateSessionId();

public:
    SessionManager();
    SessionManager(const SessionManager& other);
    SessionManager& operator=(const SessionManager& other);
    ~SessionManager();
    
    std::string createSession();
    Session* findSession(const std::string& sessionId);
    void removeSession(const std::string& sessionId);
};

#endif