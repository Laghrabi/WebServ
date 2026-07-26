#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include "webserver.hpp"
#include "Config.hpp"
#include "ListeningSocket.hpp"
#include "ClientSocket.hpp"
#include "RouteManager.hpp"

#include <map>
#include <vector>
#include <poll.h>

#define MAX_EVENTS 500

enum SockType {
    LISTENER_SOCK,
    CLIENT_SOCK
};

struct EventData {
    SockType type;
    int      fd;
};

class ConnectionManager
{
public:

    typedef std::map<int, ListeningSocket> ListenerContainer;
    typedef std::map<int, Client> ClientContainer;
    typedef std::map<int, EventData*> EventContainer;

    const Config&       m_config;
    ListenerContainer   m_listeners;
    ClientContainer     m_clients;
    EventContainer      m_events;
    int epfd;

    void createListeningSockets();

    void acceptClient(ListeningSocket& listener);
    
    void recieveClient(Client& client);
    
    void sendClient(Client& client);

    int receive(Client& client);
    
    // void send(Client& client);
    
    void disconnect(Client& client);

    void ChangeClientEvent(int fd, uint32_t event);
    // void enablePollout(int fd);

    // void disablePollout(int fd);

    void AddSocketToEpfd(int fd, SockType type, uint32_t event);

public:

    ConnectionManager(const Config& config);
    ~ConnectionManager();

    void init();
    void run();

};

#endif
