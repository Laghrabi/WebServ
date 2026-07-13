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
    
    for (UnorderedMultiMap<Server::IPort, Server>::const_iterator it = m_config.m_iport_server.begin();
    it != m_config.m_iport_server.end(); it = m_config.m_iport_server.upper_bound(it->first))
    {
        ListeningSocket listener(&it->first);
        
        int fd = socket(listener.getEndpoint().getFamily(), SOCK_STREAM, 0);
        if (fd < 0)
            throw std::runtime_error("socket failed");
        else
        {
            //test
            std::cout << "creat socket fd = " << fd << "for endpoint ";
            listener.getEndpoint().print();
            std::cout << "\n";
        }
        int yes = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
            throw std::runtime_error("setsockopt failed");
        if (bind(fd, listener.getEndpoint().get(), listener.getEndpoint().getSize()) < 0)
        {
            std::cout << (listener.getEndpoint().getFamily() ) << "\n";
            perror("bind");
            throw std::runtime_error("bind failed");
        }
        else
        {
            std::cout << "bind socket fd = " << fd << "for endpoint ";
            listener.getEndpoint().print();
            std::cout << "\n";
        }
        if (listen(fd, SOMAXCONN) < 0)
            throw std::runtime_error("listen failed");
        else 
        {
            std::cout << "socket fd = " << fd << "is listening for endpoint ";
            listener.getEndpoint().print();
            std::cout << "\n";
        }
        if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0)
            throw std::runtime_error("fcntl failed");
        else
        {
            std::cout << "socket fd = " << fd << "is set to non-blocking for endpoint ";
            listener.getEndpoint().print();
            std::cout << "\n";
        }
        listener.setFd(fd);
        m_listeners.insert(
        std::make_pair(fd, listener));
        std::cout << "listener added for endpoint " << std::endl;
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

void ConnectionManager::acceptClient(ListeningSocket& listener)
{
    sockaddr_storage address;
    socklen_t addressLength = sizeof(address);

    int clientFd = accept(listener.getFd(), reinterpret_cast<sockaddr*>(&address),
        &addressLength);

    if (clientFd < 0)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
            perror("accept");
        return;
    }

    if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
    {
        perror("fcntl");
        close(clientFd);
        return;
    }

    const Server::IPort& key = listener.getEndpoint();
    const Config::ServerMultiMap map = m_config.m_iport_server;
    Client client(clientFd, &listener, address, addressLength, map.equal_range(key));
    std::cout << "Accepted new client with fd: " << clientFd << "\n";
    m_clients.insert(
        std::make_pair(clientFd, client));
    
    struct pollfd pfd;
        
    pfd.fd = clientFd;
    pfd.events = POLLIN | POLLOUT;
    pfd.revents = 0;
    
    m_pollfds.push_back(pfd);
}


void ConnectionManager::disconnect(Client& client)
{
    int fd = client.getFd();
    std::cout << "Disconnecting client with fd: " << fd << "\n";

    close(fd);

    m_clients.erase(fd);
    std::vector<struct pollfd>::iterator it;

    for (it = m_pollfds.begin(); it != m_pollfds.end(); ++it)
    {
        if (it->fd == fd)
        {
            m_pollfds.erase(it);
            break;
        }
    }

}


int ConnectionManager::receive(Client& client)
{
    char    buffer[4096] = {0};
    ssize_t bytes;

    bytes = recv(client.getFd(), buffer, sizeof(buffer), 0);
    
    if (bytes > 0)
    {
        client.getReadBuffer().clear();
        client.getReadBuffer().insert( client.getReadBuffer().end(),
                buffer, buffer + bytes);
        return (0);
    }
    else if (bytes == 0)
    {
        disconnect(client);
        return (1);
    }
    else
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return (1);
        }
        perror("recv");
        disconnect(client);
        return (1);
    }
}

void ConnectionManager::enablePollout(int fd)
{
    for (std::size_t i = 0; i < m_pollfds.size(); ++i)
    {
        if (m_pollfds[i].fd == fd)
        {
            m_pollfds[i].events = POLLOUT;
            break;
        }
    }
}

void ConnectionManager::disablePollout(int fd)
{
    for (std::size_t i = 0; i < m_pollfds.size(); ++i)
    {
        if (m_pollfds[i].fd == fd)
        {
            m_pollfds[i].events &= ~POLLOUT;
            break;
        }
    }
}

void ConnectionManager::recieveClient(Client& client)
{
    if (receive(client))
        return;

    client.m_request.parse(client.getReadBuffer());
    int state = client.m_request.getCurrentState(); 

    // if (state == FINISHED) {
    //     RouteResult result = routeManager.processRequest(client.m_request);
    //     client.generateResponse(result);
    // } 
    // else if (state == ERROR) {
    //     int errorCode = client.m_request.getStatusCode(); 
    //     client.generateErrorResponse(errorCode);
    // }

    enablePollout(client.getFd());
}

void ConnectionManager::sendClient(Client& client)
{
    //i dont know how to catch the data that i will send to the client, i will ask chaimaa
    // i guess it will return a vector of char
    std::vector<char> response;// i will reciev data here
    send(client.getFd(), &response[0], response.size(), 0);
    disablePollout(client.getFd());
}

void ConnectionManager::run()
{
    while (true)
    {
        int ready;
        ready = poll(&m_pollfds[0], m_pollfds.size(), -1);

        if (ready < 0)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("poll() failed");
        }

        for (std::size_t i = 0; i < m_pollfds.size() && ready > 0; ++i)
        {
            short events = m_pollfds[i].revents;

            if (m_pollfds[i].revents == 0)
                continue;
            --ready;

            int fd = m_pollfds[i].fd;

            if (events & (POLLERR | POLLHUP))
            {
                disconnect(m_clients.find(fd)->second);
                continue;
            }

            if (events & (POLLIN | POLLOUT))
            {
                if (m_listeners.find(fd) != m_listeners.end() && (events & POLLIN))
                {
                    acceptClient(m_listeners.find(fd)->second);
                }          
                else if (m_clients.find(fd) != m_clients.end()  && (events & POLLIN))
                {
                    recieveClient(m_clients.find(fd)->second);
                }
                else if (m_clients.find(fd) != m_clients.end()  && (events & POLLOUT))
                {
                    sendClient(m_clients.find(fd)->second);
                }
            }
        }
    }
}