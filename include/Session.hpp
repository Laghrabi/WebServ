#ifndef SESSION_HPP
#define SESSION_HPP

#include <string>

class Session
{
private:
    std::string _sessionId;
    unsigned int _visitCount;

public:
    Session();
    Session(const std::string& sessionId);
    Session(const Session& other);
    Session& operator=(const Session& other);
    ~Session();

    const std::string& getSessionId() const;
    unsigned int getVisitCount() const;

    void visit();
};

#endif