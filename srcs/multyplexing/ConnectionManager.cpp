#include "ConnectionManager.hpp"
#include "HttpRequestHandler.hpp"
#include "HttpResponse.hpp"
#include "RouteResult.hpp"
#include <iostream>
#include <utility>


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
	std::cout << "client" << client.getFd() << "disconnect"<< std::endl;
	if (epoll_ctl(epfd, EPOLL_CTL_DEL,  client.getFd(), NULL))
	{
		perror("epoll_ctl failed to delete");
	}
	close(client.getFd());
	m_clients.erase(client.getFd());
	delete (m_events.find(client.getFd())->second);
	m_events.erase(client.getFd());
}


int ConnectionManager::receive(Client& client, int fd)
{
	char    buffer[4096] = {0};
	ssize_t bytes;
	std::cerr << "cgi fd = " << fd << "\n";
	if (client.m_pipefd == -1)
	    bytes = recv(fd, buffer, sizeof(buffer), 0);
	else
	    bytes = read(fd, buffer, sizeof(buffer));

	if (bytes > 0)
	{
		std::cerr << "there is bytes from \n";
		client.getReadBuffer().clear();
		client.getReadBuffer().insert( client.getReadBuffer().end(),
				buffer, buffer + bytes);
		return (0);
	}
	else if (bytes == 0)
	{
		std::cerr << "0 bytes cgi\n";
		disconnect(client);
		return (1);
	}
	else
	{
		std::cerr << "error cgi \n";
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			return (1);
		}
		std::cerr << " somethgin else\n";
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			perror("recv");
		disconnect(client);
		return (1);
	}
	std::cerr << "here i dont' know\n";
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

void ConnectionManager::recievePipe(Client& client)
{
	std::cerr << "i no am here\n";
	if (receive(client, client.m_pipefd))
		return;

	std::cerr << "i am here\n";
	const std::vector<char>& c = client.getReadBuffer();
	std::cout << std::string(10, '=');
	std::cout << std::string(c.begin(), c.end()) << "\n";
	std::cout << std::string(10, '=');
	std::cout << "\n";
	client.m_cgi_handler.parse(c);
}

void ConnectionManager::recieveClient(Client& client)
{
	if (receive(client, client.getFd()))
		return;
	// check if type is cgi_pipe 
	// events

	client.getRequest().parse(client.getReadBuffer());
	int state = client.getRequest().getCurrentState(); 

	HttpRequest& request = client.getRequest();
	if (state == FINISHED) {
		// routing to find resources to provide
		RouteManager route_manager;
		route_manager.processRequest(request);
		RouteResult result = request._routeResult;

		std::cout << "result status Code = " << result.statusCode << "\n";
		if (result.action != ACTION_ERROR) {
			std::cout << "target path = " << result.targetPath << "\n";

			HttpRequest::printHttpStatus(request.getStatusCode());
			RouteManager::printRouteAction(result.action);
			request.printBodyContent();
			if (result.action == ACTION_EXECUTE_CGI) {
				std::cout << "====================Action is cgi" << std::endl;
				client.m_pipefd = client.m_cgi_handler.execute();
				// if (client.m_pipefd < 0)
				//check what cgi return 
				std::cerr << "action cgi " << client.m_pipefd;
				AddSocketToEpfd(client.m_pipefd, CGI_PIPE, EPOLLIN);
				m_clients.insert(
						std::make_pair(client.m_pipefd, client));
				}
		}
		else {
			HttpRequest::printHttpStatus(request.getStatusCode());
			RouteManager::printRouteAction(result.action);
			std::cout << "error\n";

		}
	} else if (state == ERROR) {
		request.printHttpStatus(request.getStatusCode());
	} else {
		return ;
	}
	HttpResponse& response = client.getResponse();
	HttpRequestHandler handler(request, response);
	handler.handleRequest();
	ChangeClientEvent(client.getFd(), EPOLLOUT);
}

void ConnectionManager::sendClient(Client& client)
{
	HttpResponse& response = client.getResponse();
	std::vector<char> chunk = response.assembleResponse();
	std::cerr << "CHUNK SIZE = " << chunk.size() << std::endl;
	if (chunk.empty() && response.getHeadersSent())
	{
		std::cerr << "nope\n";
		response.clear();
		if (response.keep_connection == 0)
		{
			disconnect(client);
			return;
		}
		ChangeClientEvent(client.getFd(), EPOLLIN);
		return;
	}
	ssize_t n = send(client.getFd(), &chunk[0], chunk.size(), 0);
	std::cerr << "size n  = " << n << "\n";
	response.eraseSendBytes(n);
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
				else if (type == (CGI_PIPE)) {
					std::cerr << "there is cgi pipe\n";
					recievePipe(m_clients.find(fd)->second);
				}
			}
			--ready;
		}
	}
}

