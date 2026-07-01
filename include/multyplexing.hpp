#ifndef CONNECTIONMANAGER_HPP
#define CONNECTIONMANAGER_HPP

#include "webserver.hpp"
#include "Config.hpp"
#include <map>
#include <vector>
#include <poll.h>

class ConnectionManager
{
    public:

        struct ListeningSocket
        {
            int             fd;
            Server::IPort   endpoint;

            ListeningSocket();
            ~ListeningSocket();
        };

        struct Client
        {
            int                     fd;
            sockaddr_storage        address;
            socklen_t               addressLength;
            std::string             readBuffer;
            std::string             writeBuffer;
            ListeningSocket*        listener;
            Client();
            ~Client();
        };

        typedef std::vector<ListeningSocket>          ListenerContainer;
        typedef std::map<int, Client>                 ClientContainer;
        typedef std::vector<struct pollfd>            PollContainer;

        explicit ConnectionManager(const Config& config);
        ~ConnectionManager();

        void initialize();
        void run();

    private:

        const Config&      m_config;

        ListenerContainer  m_listeners;

        ClientContainer    m_clients;

        PollContainer      m_pollfds;
};

#endif