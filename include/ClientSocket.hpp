#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "webserver.hpp"
#include "ListeningSocket.hpp"

class Client
{
    private:
    
        int                 m_fd;
        sockaddr_storage    m_address;
        socklen_t           m_addressLength;
        std::string         m_readBuffer;
        std::string         m_writeBuffer;
        ListeningSocket*    m_listener;

    public:

        Client();
        Client(int fd, ListeningSocket* listener);
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();

        int getFd() const;

        std::string& getReadBuffer();
        std::string& getWriteBuffer();

        const std::string& getReadBuffer() const;
        const std::string& getWriteBuffer() const;

        void setAddress(const sockaddr_storage& address);
        void setAddressLength(socklen_t length);

        const sockaddr_storage& getAddress() const;
        socklen_t getAddressLength() const;

        ListeningSocket* getListener();
        const ListeningSocket* getListener() const;

};

#endif