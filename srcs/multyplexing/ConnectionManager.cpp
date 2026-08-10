#include "ConnectionManager.hpp"
#include "HttpResponse.hpp"
#include <cerrno>
#include <cstdio>
#include <sys/epoll.h>


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
	std::memset(evdata, 0, sizeof(EventData));

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
			std::cout << "creat socket fd = " << fd << " for endpoint ";
			listener.getEndpoint().print();
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
		if (listen(fd, SOMAXCONN) < 0)
			throw std::runtime_error("listen failed");
		else 
		{
			std::cout << "socket fd = " << fd << " is listening for endpoint ";
			listener.getEndpoint().print();
		}
		listener.setFd(fd);
		m_listeners.insert(
				std::make_pair(fd, listener));
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
	Client client(clientFd, &listener, Server::IPort(address), map.equal_range(key), m_config);
	std::cout << "[ACCEPT]: Accepted new client with fd: " << clientFd << std::endl;
	m_clients.insert(
			std::make_pair(clientFd, client));

	AddSocketToEpfd(clientFd, CLIENT_SOCK, EPOLLIN);
}


void ConnectionManager::disconnect(Client& client)
{
	std::cout << "[DISCONNECT]: "<< "client " << client.getFd() << " disconnect"<< std::endl;


	// if (client.m_pipefd != -1)
	// {
	// 	epoll_ctl(epfd, EPOLL_CTL_DEL,  client.m_pipefd, NULL);
	// 	m_events.erase(client.m_pipefd);
	// 	delete (m_events.find(client.m_pipefd)->second);
	// 	m_client_pipes.erase(client.m_pipefd);
	// 	safeClose(client.m_pipefd);
	// 	client.m_pipefd = -1;
	// }
	if (epoll_ctl(epfd, EPOLL_CTL_DEL,  client.getFd(), NULL))
	{
		perror("epoll_ctl failed to delete");
	}
	int clientFd = client.getFd();
	delete (m_events.find(clientFd)->second);
	m_events.erase(clientFd);
	m_clients.erase(clientFd);
	safeClose(clientFd);
}


int ConnectionManager::receive(Client& client, int fd)
{
	char    buffer[SENDSIZE + 1] = {0};
	ssize_t bytes;
	std::cout << "cgi fd = " << fd << "\n";
	// NOTE: DO SOMETHING HERE that is special to pipe
	// if (client.m_pipefd == -1)
	// {
	    bytes = recv(fd, buffer, SENDSIZE, 0);
		std::cout << "[RECV]: from client" << fd << buffer << std::endl;
	// }
	// else
	// {
	//     bytes = read(fd, buffer, SENDSIZE);
	// 	std::cout << "[RECV]: from pipe" << buffer << std::endl;
	// 	if (bytes == 0) {
	// 		std::cout << "==========================************************================\n";
	// 	}
	// }

	if (bytes > 0)
	{
		client.getReadBuffer().clear();
		client.getReadBuffer().insert( client.getReadBuffer().end(),
				buffer, buffer + bytes);
		return (0);
	}
	else if (bytes == 0)
	{
		std::cout << "[RECV]: client " << client.getFd() << "close the connection";
		disconnect(client);
		return (1);
	}
	else
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
		{
			return (1);
		}
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			perror("recv");
		std::cout << "desconnecting\n";
		disconnect(client);
		return (1);
	}
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

void ConnectionManager::receivePipe(Client& client, int fd)
{
	char    buffer[SENDSIZE + 1] = {0};
	ssize_t bytes;
	std::cout << "client m_pipefd = " << fd << std::endl;

	bytes = read (fd, buffer, SENDSIZE);
	if (bytes == 0) {
		std::cout << "[recieve pipe] 0 byte\n";
	}
	else if (bytes < 0) {
		perror("recieve pipe");
		std::cout << (EINTR == errno) << "\n";
		std::cout << "[recieve pipe] < 0 byte\n";
		return ;
	}
	std::cout << std::string(100, '=') << "\n";
	std::cout << buffer << "\n";
	std::cout << std::string(100, '=') << "\n";
	std::vector<char> vec(buffer, buffer + bytes);
	client.m_cgi_handler.parse(vec);
}

void ConnectionManager::handleCgi(Client& client) { 
		std::cout << "[CGI] this action is cgi" << std::endl;
		client.m_pipefd = client.m_cgi_handler.execute();
		std::cout << "pipe fd = " << client.m_pipefd << "\n";
			if (client.m_pipefd < 0) {
				//call the make error function here
// exit(20);
			}
			else {
			//check what cgi return 
			std::cerr << "action cgi " << client.m_pipefd;
			AddSocketToEpfd(client.m_pipefd, CGI_PIPE, EPOLLIN);
			m_client_pipes.insert(
			std::make_pair(client.m_pipefd, &client));
	}
}



void ConnectionManager::receiveClient(Client& client)
{
	std::cout << "hadi dyal lclient" << std::endl;
	if (receive(client, client.getFd()))
		return;

	client.getRequest().parse(client.getReadBuffer());
	int state = client.getRequest().getCurrentState(); 
	HttpRequest& request = client.getRequest();
	if (state == FINISHED) {
		std::cout << "[receive]: http request recieved completly" << std::endl; 
		request.debugPrintHeaders();
		request.printBodyContent();
		// RouteManager route_manager;
		// route_manager.processRequest(request);
		RouteResult result = request._routeResult;
		RouteManager::printRouteAction(result.action);
		HttpRequest::printHttpStatus(result.statusCode);
		std::map<HttpStatus, std::string>::const_iterator it = client.getResponse().getStatusCodeMap().find(result.statusCode);
		std::cout << "result status Code = " << it->second << "\n";
		std::cout << "target path = " << result.targetPath << "\n";
			if (result.action == ACTION_EXECUTE_CGI) {
				handleCgi(client);
			}
		
	} else if (state == ERROR) {
		request.printHttpStatus(request.getStatusCode());
	} else {
		return ;
	}
	HttpResponse& response = client.getResponse();
	HttpRequestHandler handler(request, response);
	std::cout << request.getStatusCode() << "\n";
	handler.handleRequest();
	ChangeClientEvent(client.getFd(), EPOLLOUT);
}



void ConnectionManager::sendClient(Client& client)
{
	client.checkCgiState();
	HttpResponse& response = client.getResponse();
	size_t size = response.assembleResponse();

	if (size != 0 && response.is_ok_send) {
		ssize_t n = send(client.getFd(), &response.buffer[0], size, 0);
		response.eraseSendBytes(n);
	}
	// std::cout << "it segefault here" << std::endl;

	// std::cout << response.is_finished<< std::endl;
	if (response.is_finished)
	{
		if (client.getRequest().getCurrentState() == FINISHED) {
			std::cout << "[CONNECTION MANGER] write access log\n";
			response.setLog(client.getRequest());
		}
		
		client.getRequest().removeTmpFile();
		response.clear();
		// client.m_cgi_handler.
		if (response.keep_connection == 0)
		{
			std::cout << "disconnecting from sendClient\n";
			disconnect(client);
			return;
		}
		std::cout << "[multiplexing] return to EPOLLIN\n";
		ChangeClientEvent(client.getFd(), EPOLLIN);
		client = Client(client.getFd(), client.getListener(), 
				client.getRequest().getClientIPort(), client.getRequest().getServerRange(), 
				response.config);
		return;
	}

}

// void ConnectionManager::deleteCgi(Client& client)
// {
// 	client.checkCgiState();
// 	std::cout << "[CGI] deleting cgi resources\n";
// 	std::cout << "cgi fd " << client.m_pipefd << "\n";
// 	if (client.m_pipefd != -1)
// 	{
// 		epoll_ctl(epfd, EPOLL_CTL_DEL,  client.m_pipefd, NULL);
// 		delete (m_events.find(client.m_pipefd)->second);
// 		m_events.erase(client.m_pipefd);
// 		m_client_pipes.erase(client.m_pipefd);
// 		safeClose(client.m_pipefd);
// 		client.m_pipefd = -1;
// 	}
// }
void ConnectionManager::deleteCgi(EventData *data, int fd)
{
	// client.checkCgiState();
	std::cout << "[CGI] deleting cgi resources\n";
	std::cout << "cgi fd " << fd << "\n";

		epoll_ctl(epfd, EPOLL_CTL_DEL,  fd, NULL);
		delete (data);
		m_events.erase(fd);
		m_client_pipes.erase(fd);
		safeClose(fd);
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
			// std::cout << "fd = " << fd << "\n";
			// std::cout << (events & (EPOLLERR | EPOLLHUP)) << "\n";
			// if (m_events.count(fd) == 0)
			// {
			// 	// exit (20);
			// 	--ready;
			// 	continue;
			// }
			if (events & (EPOLLERR | EPOLLHUP) && type == CLIENT_SOCK)
			{
				std::cout << "client send disconnect epollhub\n";
				disconnect(m_clients.find(fd)->second);
				--ready;
				continue;
			}
			if ((events & (EPOLLHUP | EPOLLERR)) && type == CGI_PIPE)
			{
				char buff[SENDSIZE];
				int i = read(fd, buff, SENDSIZE);
				write(1, buff, i);
				std::cout << "read i = " << i << "\n";
				std::cout << EPOLLHUP << "\n";
				deleteCgi(data, fd);

				std::cout << "EPOLLHUB                            KLASJFKLASJDFLKJASKLDFJALKSDJF\n";
				--ready;
				continue;
			}
			else if(events & (EPOLLIN | EPOLLOUT))
			{
				if (type == LISTENER_SOCK && (events & EPOLLIN))
					acceptClient(m_listeners.find(fd)->second);
				else if (type == CLIENT_SOCK && (events & EPOLLIN)) {
					std::cout << "file descriptor = " << fd << "\n";
					receiveClient(m_clients.find(fd)->second);
				}
				else if (type == CLIENT_SOCK && (events & EPOLLOUT)) {
					// std::cout << fd << "\n";
					sendClient(m_clients.find(fd)->second);
				}
				else if (type == (CGI_PIPE)) {
					std::cout << "hehehalsdfklasdjflk" << (events & EPOLLIN) << std::endl;
					// NOTE: something here check if there is client
					std::map<int, Client*>::iterator it = m_client_pipes.find(fd);
					if (it != m_client_pipes.end())
					{
						// std::cout << m_clients.at(5).m_pipefd << "\n";
						std::cout << "[IMPORTNAT]fd = " << fd << " " << m_client_pipes.count(fd) << "\n";
						receivePipe(*it->second, fd);
					}
				}
			}
			--ready;
		}
	}
}

