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
        std::vector<char> m_readBuffer;
        std::vector<char> m_writeBuffer;
        ListeningSocket*    m_listener;
        
    public:
        
        HttpRequest           m_request;
        Client() ;
        Client(int fd, ListeningSocket* listener, const sockaddr_storage& address,
                    socklen_t addressLength, const Config::ServerRange& serverRange) ;
        Client(const Client& other);
        Client& operator=(const Client& other);
        ~Client();

        int getFd() const;

        std::vector<char>& getReadBuffer();
        std::vector<char>& getWriteBuffer();

        const std::vector<char>& getReadBuffer() const;
        const std::vector<char>& getWriteBuffer() const;

        void setAddress(const sockaddr_storage& address);
        void setAddressLength(socklen_t length);
        void setListener(ListeningSocket* listener);

        const sockaddr_storage& getAddress() const;
        socklen_t getAddressLength() const;

        ListeningSocket* getListener();
        const ListeningSocket* getListener() const;

        std::string getIp() const;
        uint16_t getPort() const;
};

#endif