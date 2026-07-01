#include "ConnectionManager.hpp"
#include "webserver.hpp"

ConnectionManager::ConnectionManager(const Config& config)
    : m_config(config)
{
}

ConnectionManager::~ConnectionManager()
{
}



void ConnectionManager::createListeningSockets()
{
    UnorderedMultiMap<Server::IPort, Server>::const_iterator it;
    for (it = m_config.m_iport_server.begin();
    it != m_config.m_iport_server.end(); it.nextBound())
    {
        ListeningSocket listener(it->first);
        
        int fd = socket(listener.getEndpoint().getFamily(), SOCK_STREAM, 0);
        if (fd < 0)
        throw std::runtime_error("socket failed");
        listener.setFd(fd);
        int yes = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
        throw std::runtime_error("setsockopt failed");
        
        if (bind(fd, listener.getEndpoint().get(), listener.getEndpoint().getSize()) < 0)
        throw std::runtime_error("bind failed");
        
        if (listen(fd, SOMAXCONN) < 0)
        throw std::runtime_error("listen failed");
        
        if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("fcntl failed");
        
        m_listeners.insert(
        std::make_pair(fd, listener));
    }
}


void ConnectionManager::buildPollFds()
{
    m_pollfds.clear();
    
    ListenerContainer::const_iterator it;
    
    for (it = m_listeners.begin(); it != m_listeners.end(); ++it)
    {
        struct pollfd pfd;
        
        pfd.fd = it->first;
        pfd.events = POLLIN;
        pfd.revents = 0;
        
        m_pollfds.push_back(pfd);
    }
}

void ConnectionManager::init()
{
    createListeningSockets();
    buildPollFds();
}