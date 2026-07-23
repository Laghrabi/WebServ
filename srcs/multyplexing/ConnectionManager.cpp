#include "ConnectionManager.hpp"
#include "webserver.hpp"

ConnectionManager::ConnectionManager(const Config& config)
    : m_config(config)
{
}

ConnectionManager::~ConnectionManager()
{
    for (ListenerContainer::iterator it = m_listeners.begin(); it != m_listeners.end(); it ++)
    {
        close (it->first);
    }
    for (ClientContainer::iterator it = m_clients.begin(); it != m_clients.end(); it ++)
    {
        close (it->first);
    }
    for (EventContainer::iterator it = m_events.begin(); it != m_events.end(); ++it)
    {
        delete it->second;
    }
    if (epfd != -1)
        close(epfd);
}

void ConnectionManager::AddSocketToEpfd(int fd, SockType type, uint32_t event)
{
    struct epoll_event ev;
    struct EventData* evdata = new EventData;

    evdata->fd = fd;
    evdata->type = type;
    ev.events = event;
    ev.data.ptr = evdata;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1)
    {
        perror("epoll_ctl failed");
        delete evdata;
        return;
    }
    m_events.insert(std::make_pair(fd, evdata));
}
void ConnectionManager::createListeningSockets()
{
    m_events.clear();
    epfd = epoll_create1(0);
    if (epfd == -1)
        throw std::runtime_error("epoll_create1 failed");
    for (UnorderedMultiMap<Server::IPort, Server>::const_iterator it = m_config.m_iport_server.begin();
    it != m_config.m_iport_server.end(); it = m_config.m_iport_server.upper_bound(it->first))
    {
        ListeningSocket listener(&it->first);
        
        int fd = socket(listener.getEndpoint().getFamily(), SOCK_STREAM, 0);
        if (fd < 0)
            throw std::runtime_error("socket failed");
        else
        {
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
        listener.setFd(fd);
        m_listeners.insert(
        std::make_pair(fd, listener));
        std::cout << "listener added for endpoint " << std::endl;
        AddSocketToEpfd(listener.getFd(), LISTENER_SOCK, EPOLLIN);
    }
}


void ConnectionManager::init()
{
    createListeningSockets();
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

    const Server::IPort& key = listener.getEndpoint();
    const Config::ServerMultiMap& map = m_config.m_iport_server;
    Client client(clientFd, &listener, Server::IPort(address), map.equal_range(key));
    std::cout << "Accepted new client with fd: " << clientFd << "\n";
    m_clients.insert(
        std::make_pair(clientFd, client));
    
    AddSocketToEpfd(clientFd, CLIENT_SOCK, EPOLLIN);
}


void ConnectionManager::disconnect(Client& client)
{
    if (epoll_ctl(epfd, EPOLL_CTL_DEL,  client.getFd(), NULL))
    {
        perror("epoll_ctl failed to delete");
    }
    close(client.getFd());
    m_clients.erase(client.getFd());
    delete (m_events.find(client.getFd())->second);
    m_events.erase(client.getFd());
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

void ConnectionManager::ChangeClientEvent(int fd, uint32_t event)
{
    struct epoll_event ev;
    EventContainer::iterator it = m_events.find(fd);
    if (it == m_events.end())
    {
        throw std::runtime_error("Fd doesnt exist in epoll");
    }
    ev.events = event;
    ev.data.ptr = it->second;
    if (epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev))
    {
        perror("epoll_ctl failed");
    }
}

void ConnectionManager::recieveClient(Client& client)
{
    if (receive(client))
        return;

    client.getRequest().parse(client.getReadBuffer());
    // int state = client.m_request.getCurrentState(); 

    // if (state == FINISHED) {
    //     RouteResult result = routeManager.processRequest(client.m_request);
    //     client.generateResponse(result);
    // } 
    // else if (state == ERROR) {
    //     int errorCode = client.m_request.getStatusCode(); 
    //     client.generateErrorResponse(errorCode);
    // }
    ChangeClientEvent(client.getFd(), EPOLLOUT);
}

void ConnectionManager::sendClient(Client& client)
{

    //i dont know how to catch the data that i will send to the client, i will ask chaimaa
    // and i need to ask someone how much byte normaly i need to pass on a single send()
    const std::vector<char>& buf = client.getWriteBuffer();
    size_t sent = static_cast<size_t>(client.getBytesSent());

    if (buf.empty() || sent >= buf.size())
    {
        client.setByteSent(0);
        client.getWriteBuffer().clear();
        ChangeClientEvent(client.getFd(), EPOLLIN);
        return;
    }

    size_t remaining = buf.size() - sent;

    ssize_t n = send(client.getFd(), &buf[sent],
        std::min(remaining, static_cast<size_t>(4096)), MSG_NOSIGNAL);
    if (n > 0)
        client.setByteSent(static_cast<int>(sent + static_cast<size_t>(n)));
    else if (n == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return;
        disconnect(client);
        return;
    }

    if (static_cast<size_t>(client.getBytesSent()) >= client.getWriteBuffer().size())
    {
        client.setByteSent(0);
        client.getWriteBuffer().clear();
        ChangeClientEvent(client.getFd(), EPOLLIN);
    }
    //anyway i still dont know if im done with the multiplexing,
    // waiting ofr the respond to start testing
}

void ConnectionManager::run()
{
    struct epoll_event evlist[MAX_EVENTS];
    while (true)
    {
        int ready;

        ready = epoll_wait(epfd, evlist, MAX_EVENTS, -1);
        if (ready < 0)
        {
            if (errno == EINTR)
                continue;
            throw std::runtime_error("epoll() failed");
        }

        for (std::size_t i = 0; i < MAX_EVENTS && ready > 0; ++i)
        {
            short events = evlist[i].events;
            EventData *data = static_cast<EventData *>(evlist[i].data.ptr);
            int fd = data->fd;
            int type = data->type;

            //i need to handle the cgi fd here...


            if (events & (EPOLLERR | EPOLLHUP) && type == CLIENT_SOCK)// should do the same fo the cgi  because it will be type  CGI_SOCK
            {
                disconnect(m_clients.find(fd)->second);
                --ready;
                continue;
            }
            else if(events & (EPOLLIN | EPOLLOUT))
            {
                if (type == LISTENER_SOCK && (events & EPOLLIN))
                    acceptClient(m_listeners.find(fd)->second);
                else if (type == CLIENT_SOCK && (events & EPOLLIN))
                    recieveClient(m_clients.find(fd)->second);
                else if (type == CLIENT_SOCK && (events & EPOLLOUT))
                    sendClient(m_clients.find(fd)->second);
            }
            --ready;
        }
    }
}

