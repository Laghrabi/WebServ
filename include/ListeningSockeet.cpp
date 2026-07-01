#include "ListeningSocket.hpp"

ListeningSocket::ListeningSocket()
    : m_fd(-1),
      m_endpoint()
{
}

ListeningSocket::ListeningSocket(const Server::IPort& endpoint)
    : m_fd(-1),
      m_endpoint(endpoint)
{
}

ListeningSocket::ListeningSocket(const ListeningSocket& other)
    : m_fd(other.m_fd),
      m_endpoint(other.m_endpoint)
{
}

ListeningSocket& ListeningSocket::operator=(const ListeningSocket& other)
{
    if (this != &other)
    {
        m_fd = other.m_fd;
        m_endpoint = other.m_endpoint;
    }
    return (*this);
}

ListeningSocket::~ListeningSocket()
{
}

int ListeningSocket::getFd()
{
    return (m_fd);
}

void ListeningSocket::setFd(int fd)
{
    m_fd = fd;
}

const Server::IPort& ListeningSocket::getEndpoint() const
{
    return (m_endpoint);
}

Server::IPort& ListeningSocket::getEndpoint()
{
    return (m_endpoint);
}

bool ListeningSocket::isOpen() const
{
    return (m_fd >= 0);
}