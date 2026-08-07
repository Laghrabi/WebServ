#ifndef CLIENTSOCKET_HPP
#define CLIENTSOCKET_HPP

#include "webserver.hpp"
#include "ListeningSocket.hpp"
#include "CgiHandler.hpp"

class Client
{
    private:
    
        int                 m_fd;
        std::vector<char>   m_readBuffer;
        std::vector<char>   m_writeBuffer;
        int                 bytesSent;
        ListeningSocket*    m_listener;
        HttpRequest         m_request;
        HttpResponse        m_response;
        
      public:
        
        int                 m_pipefd;
        Client(int fd, ListeningSocket* listener, Server::IPort ClientEndPoint, const Config::ServerRange& serverRange, const Config& config);
        Client(const Client& other);
        ~Client();

        int getFd() const;
				CgiHandler					m_cgi_handler;

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
        HttpResponse& getResponse();

        void checkCgiState();
};

#endif
