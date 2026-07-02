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
        std::cout << "hello  awald l97ba\n";
        ListeningSocket listener(it->first);
        
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
            throw std::runtime_error("bind failed");
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

    Client client(clientFd, &listener);
    std::cout << "Accepted new client with fd: " << clientFd << "\n";
    client.setAddress(address);
    client.setAddressLength(addressLength);

    m_clients.insert(
        std::make_pair(clientFd, client));
}

void ConnectionManager::disconnect(Client& client)
{
    int fd = client.getFd();

    close(fd);

    m_clients.erase(fd);
}

void ConnectionManager::receive(Client& client)
{
    char    buffer[4096];
    ssize_t bytes;

    while (true)
    {
        bytes = recv(client.getFd(), buffer, sizeof(buffer), 0);

        if (bytes > 0)
        {
            client.getReadBuffer().insert( client.getReadBuffer().end(),
                    buffer, buffer + bytes);
        }
        else if (bytes == 0)
        {
            disconnect(client);
            return;
        }
        else
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            
            perror("recv");
            disconnect(client);
            return;
        }
    }
}

void ConnectionManager::handleClient(Client& client)
{
    receive(client);

    //i need to know what should i pass to the http request handler, 
    //i think i should pass the second value of the unordredmultimap which is the server object,
    //so probably i will pass config.find(client.getListener()->getEndpoint())
    //to the http request handler to get the servers that could handle this request, 
    //and also the readbuffer of the client and config object as i remember 

    //and i still have to work on the send function to send the response back to the client
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
                    acceptClient(m_listeners.find(fd)->second);
                else
                    handleClient(m_clients.find(fd)->second);
            }

            // if (events & POLLOUT)
            // {
            //     sendToClient(fd);
            // }

        }
    }
}