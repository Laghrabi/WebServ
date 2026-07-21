#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "webserver.hpp"
#include "ListeningSocket.hpp"

class Client
{
    private:
    
        int                 m_fd;
        std::vector<char>   m_readBuffer;
        std::vector<char>   m_writeBuffer;
        int                 bytesSent;
        ListeningSocket*    m_listener;
        HttpRequest         m_request;
        
    public:
        
        Client(int fd, ListeningSocket* listener, Server::IPort ClientEndPoint, const Config::ServerRange& serverRange) ;
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();

        int getFd() const;

        std::vector<char>& getReadBuffer();
        std::vector<char>& getWriteBuffer();
        int getBytesSent();

        const std::vector<char>& getReadBuffer() const;
        const std::vector<char>& getWriteBuffer() const;

        void setListener(ListeningSocket* listener);
        void setByteSent(int total);

        ListeningSocket* getListener();
        const ListeningSocket* getListener() const;

        HttpRequest& getRequest();
};

#endif