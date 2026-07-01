#include "ClientSocket.hpp"

Client::Client(): m_fd(-1),
    m_address(),
    m_addressLength(sizeof(sockaddr_storage)),
    m_readBuffer(),
    m_writeBuffer(),
    m_listener(NULL)
{
}

Client::Client(int fd, ListeningSocket* listener):
    m_fd(fd),
    m_address(),
    m_addressLength(sizeof(sockaddr_storage)),
    m_readBuffer(),
    m_writeBuffer(),
    m_listener(listener)
{
}

Client::Client(const Client& other):
    m_fd(other.m_fd),
    m_address(other.m_address),
    m_addressLength(other.m_addressLength),
    m_readBuffer(other.m_readBuffer),
    m_writeBuffer(other.m_writeBuffer),
    m_listener(other.m_listener)
{
}

Client& Client::operator=(const Client& other)
{
    if (this != &other)
    {
        m_fd = other.m_fd;
        m_address = other.m_address;
        m_addressLength = other.m_addressLength;
        m_readBuffer = other.m_readBuffer;
        m_writeBuffer = other.m_writeBuffer;
        m_listener = other.m_listener;
    }
    return (*this);
}

Client::~Client()
{
}

void Client::setAddress(const sockaddr_storage& address)
{
    m_address = address;
}

void Client::setAddressLength(socklen_t length)
{
    m_addressLength = length;
}

const sockaddr_storage& Client::getAddress() const
{
    return (m_address);
}

socklen_t Client::getAddressLength() const
{
    return (m_addressLength);
}

int Client::getFd() const
{
    return (m_fd);
}

std::string& Client::getReadBuffer()
{
    return (m_readBuffer);
}

std::string& Client::getWriteBuffer()
{
    return (m_writeBuffer);
}

const std::string& Client::getReadBuffer() const
{
    return (m_readBuffer);
}

const std::string& Client::getWriteBuffer() const
{
    return (m_writeBuffer);
}

ListeningSocket* Client::getListener()
{
    return (m_listener);
}

const ListeningSocket* Client::getListener() const
{
    return (m_listener);
}