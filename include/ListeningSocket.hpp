#ifndef LISTENINGSOCKET_HPP
#define LISTENINGSOCKET_HPP

#include "webserver.hpp"
#include "Server.hpp"

class ListeningSocket
{
    public:

        ListeningSocket();
        ListeningSocket(const Server::IPort& endpoint);
        ListeningSocket(const ListeningSocket& other);
        ListeningSocket& operator=(const ListeningSocket& other);
        ~ListeningSocket();

        int getFd();
        void setFd(int fd);
        const Server::IPort& getEndpoint() const;
        Server::IPort& getEndpoint();

        bool isOpen() const;

    private:

        int             m_fd;
        Server::IPort   m_endpoint;
};

#endif