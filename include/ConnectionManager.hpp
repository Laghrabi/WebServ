#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include "webserver.hpp"
#include "Config.hpp"
#include "ListeningSocket.hpp"
#include "ClientSocket.hpp"

#include <map>
#include <vector>
#include <poll.h>

class ConnectionManager
{
public:

    typedef std::map<int, ListeningSocket> ListenerContainer;
    typedef std::map<int, Client> ClientContainer;
    typedef std::vector<struct pollfd> PollContainer;

    const Config&       m_config;
    ListenerContainer   m_listeners;
    ClientContainer     m_clients;
    PollContainer       m_pollfds;

    void createListeningSockets();
    
    void buildPollFds();
    
    void acceptClient(ListeningSocket& listener);
    
    void receive(Client& client);
    
    void send(Client& client);
    
    void disconnect(Client& client);

public:

    ConnectionManager(const Config& config);
    ~ConnectionManager();

    void iGnit();
    void run();

};

#endif