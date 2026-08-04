#include "../../include/ClientSocket.hpp"


Client::Client(
int fd,
ListeningSocket* listener,
Server::IPort iport,
const Config::ServerRange& serverRange,
const Config& config)
:
m_fd(fd),
m_readBuffer(),
m_writeBuffer(),
m_listener(listener),
m_request(serverRange, iport),
m_response(config),
m_pipefd(-1),
m_config(config),
m_cgi_handler(m_request)
{
}

Client::Client(const Client& other):
    m_fd(other.m_fd),
    m_readBuffer(other.m_readBuffer),
    m_writeBuffer(other.m_writeBuffer),
    m_listener(other.m_listener),
    m_request(other.m_request),
    m_response(other.m_config),
	m_pipefd(other.m_pipefd),
    m_config(other.m_config),
	m_cgi_handler(m_request)
{
}

Client& Client::operator=(const Client& other)
{
    if (this != &other)
    {
        m_fd = other.m_fd;
        m_readBuffer = other.m_readBuffer;
        m_writeBuffer = other.m_writeBuffer;
        m_listener = other.m_listener;
        m_request = other.m_request;
				m_pipefd = other.m_pipefd;
    }
    return (*this);
}

Client::~Client()
{
}


void Client::setListener(ListeningSocket* listener)
{
    m_listener = listener;
}

void Client::setByteSent(int total)
{
    bytesSent = total;
}

int Client::getBytesSent()
{
    return (bytesSent);
}

int Client::getFd() const
{
    return (m_fd);
}

std::vector<char>& Client::getReadBuffer()
{
    return (m_readBuffer);
}

std::vector<char>& Client::getWriteBuffer()
{
    return (m_writeBuffer);
}

const std::vector<char>& Client::getReadBuffer() const
{
    return (m_readBuffer);
}

const std::vector<char>& Client::getWriteBuffer() const
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

HttpRequest& Client::getRequest()
{
    return (m_request);
}
HttpResponse& Client::getResponse()
{
    return (m_response);
}
