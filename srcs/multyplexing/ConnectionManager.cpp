#include "ConnectionManager.hpp"
#include "HttpResponse.hpp"
#include "webserver.hpp"
#include <cerrno>
#include <cstdio>
#include <sys/epoll.h>


ConnectionManager::ConnectionManager(const Config& config)
	: m_config(config), m_session()
{
}

ConnectionManager::~ConnectionManager()
{
	std::cout << "server recieve SIGINT" << std::endl << "freeing ...." << std::endl;
	for (EventContainer::iterator it = m_events.begin(); it != m_events.end(); it++)
	{
		delete(it->second);
	}
	for (ListenerContainer::iterator it = m_listeners.begin(); it != m_listeners.end(); it++)
	{
		if (epoll_ctl(epfd, EPOLL_CTL_DEL,  it->first, NULL))
		{
			perror("epoll_ctl failed to delete");
		}
		close(it->first);
	}
	for (ClientContainer::iterator it = m_clients.begin(); it != m_clients.end(); it++)
	{
		if (it->second.m_pipefd != -1) {
			it->second.m_cgi_handler.killProcess();
			safeClose(it->second.m_pipefd);
		}
		if (epoll_ctl(epfd, EPOLL_CTL_DEL,  it->first, NULL))
		{
			perror("epoll_ctl failed to delete");
		}
		safeClose (it->first);
	}
	for (PipeContainer::iterator it = m_client_pipes.begin(); it != m_client_pipes.end(); it++)
	{
		if (epoll_ctl(epfd, EPOLL_CTL_DEL,  it->first, NULL))
		{
			perror("epoll_ctl failed to delete");
		}
		safeClose(it->first);
	}
	if (epfd != -1)
		safeClose(epfd);
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
			std::cout << "[INIT]: creat socket fd = " << fd << " for endpoint ";
			listener.getEndpoint().print();
		}
		if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
			throw std::runtime_error("fcntl F_SETFL failed");
		else 
			std::cout << "[INIT]: set the socket " << fd << " to non-blocking" << std::endl;
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
			std::cout << "[INIT]: socket fd = " << fd << " is listening for endpoint ";
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
		perror("accept");
		return;
	}
	std::cout << "[ACCEPT]: accept client" << clientFd << std::endl;
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl");
		close(clientFd);
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

	if (client.m_cgi_handler.isAlive())
		client.m_cgi_handler.killProcess();

	handleCgiDeath(client.m_pipefd);

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
	bytes = recv(fd, buffer, SENDSIZE, 0);

	if (bytes > 0)
	{
		client.getReadBuffer().clear();
		client.getReadBuffer().insert( client.getReadBuffer().end(),
				buffer, buffer + bytes);
		return (0);
	}
	else if (bytes == 0)
	{
		std::cout << "[RECV]: client " << client.getFd() << "close the connection" << std::endl;
		disconnect(client);
		return (1);
	}
	else
	{
		std::cout << "[RECV] : recv failed" << "desconnecting\n";
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

bool ConnectionManager::receivePipe(Client& client, int fd)
{
	char    buffer[SENDSIZE + 1] = {0};
	ssize_t bytes;

	bytes = read (fd, buffer, SENDSIZE);
	if (bytes == 0) {
		std::cout << "[recieve pipe] 0 byte\n";
		return (false);
	}
	else if (bytes < 0) {
		perror("recieve pipe");
		std::cout << "[recieve pipe] < 0 byte\n";
		return false;
	}

	std::vector<char> vec(buffer, buffer + bytes);
	client.m_cgi_handler.parse(vec);
	return (true);
}

void ConnectionManager::handleCgi(Client& client) { 
	std::cout << "[CGI] this action is cgi" << std::endl;
	client.m_pipefd = client.m_cgi_handler.execute();
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
	if (receive(client, client.getFd()))
		return;
	
	Session *session = NULL;
	client.getRequest().parse(client.getReadBuffer());
	int state = client.getRequest().getCurrentState(); 
	HttpRequest& request = client.getRequest();
	if (state == FINISHED) {
		std::cout << "[receive]: http request recieved completly" << std::endl; 
		request.debugPrintHeaders();
		request.printBodyContent();

		std::string cookieHeader = request.getHeader("cookie");
		Cookies& cookie = client.getCookies(); 

		if (!cookieHeader.empty())
		{
			cookie.parse(cookieHeader);
		}

		if (cookie.has("session_id"))
		{
			std::cout << "[COOKIE]: session_id = " << cookie.get("session_id")<< std::endl;
		}
		std::string sessionId;

		if (!cookieHeader.empty() && cookie.has("session_id"))
		{
			sessionId = cookie.get("session_id");
			session = m_session.findSession(sessionId);
		}

		if (session == NULL)
		{
			sessionId = m_session.createSession();
			session = m_session.findSession(sessionId);
		}
		session->visit();
		std::cout << "[COOKIES AND SESSION MANAGEMET]: number of times this client visit our server: " << session->getVisitCount();
		RouteResult result = request._routeResult;
		RouteManager::printRouteAction(result.action);
		HttpRequest::printHttpStatus(result.statusCode);
		std::map<HttpStatus, std::string>::const_iterator it = client.getResponse().getStatusCodeMap().find(result.statusCode);
		std::cout << "[DEBUGGING]: result status Code = " << it->second << "\n";
		std::cout << "[DEBUGGING]: target path = " << result.targetPath << "\n";
		if (result.action == ACTION_EXECUTE_CGI) {
			handleCgi(client);
		}
	} else if (state == ERROR) {
		request.printHttpStatus(request.getStatusCode());
	} else {
		return ;
	}

	HttpResponse& response = client.getResponse();
	HttpRequestHandler handler(request, response, session);

	handler.handleRequest();
	ChangeClientEvent(client.getFd(), EPOLLOUT);
}


void ConnectionManager::handleCgiDeath(int fd) {
	if (fd != -1) {

		std::map<int, Client*>::const_iterator it = m_client_pipes.find(fd);
		if (it != m_client_pipes.end())
		{
			epoll_ctl(epfd, EPOLL_CTL_DEL,  fd, NULL);
			m_client_pipes.erase(fd);
			safeClose(fd);
		}
	}
}

void ConnectionManager::sendClient(Client& client)
{
	client.checkCgiState();
	HttpResponse& response = client.getResponse();
	size_t size = response.assembleResponse();
	if (size != 0 && response.is_ok_send) {
		ssize_t n = send(client.getFd(), &response.buffer[0], size, MSG_NOSIGNAL);
		if (n == -1)
		{
			disconnect(client);
			return;
		}
		if (n == 0)
			return;
		response.eraseSendBytes(n);
		std::cout << "[SEND] sending data " << n << "\n";
		return ;
	}
	if (response.getHeadersSent() && response.is_finished && response.buffer.empty())
	{
		if (client.getRequest().getCurrentState() == FINISHED) {
			std::cout << "[SEND] write access log\n";
			response.setLog(client.getRequest());
		}

		client.getRequest().removeTmpFile();
		response.clear();
		if (response.keep_connection == 0)
		{
			std::cout << "[DEBUGGING]:client " << client.getFd() << "closed the connection" << std::endl;
			disconnect(client);
			return;
		}
		else {
			handleCgiDeath(client.m_pipefd);
		}
		std::cout << "[multiplexing] return to EPOLLIN\n";
		ChangeClientEvent(client.getFd(), EPOLLIN);
		client = Client(client.getFd(), client.getListener(), 
				client.getRequest().getClientIPort(), client.getRequest().getServerRange(), 
				response.config);
		return;
	}

}


void ConnectionManager::deleteCgi(EventData *data, int fd)
{
	std::cout << "[CGI] deleting cgi resources\n";

	epoll_ctl(epfd, EPOLL_CTL_DEL,  fd, NULL);
	delete (data);
	m_events.erase(fd);
	safeClose(fd);
}

volatile sig_atomic_t g_running = 1;

void handleSignal(int signal)
{
	(void)signal;
	g_running = 0;
}

void ConnectionManager::run()
{
	struct epoll_event evlist[MAX_EVENTS];

	signal(SIGINT, handleSignal);

	while (true)
	{
		int ready;
		if (!g_running)
		{
			return;
		}

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
			if (events & (EPOLLERR | EPOLLHUP) && type == CLIENT_SOCK)
			{
				std::cout << "[DISCONNECT]: client send disconnect epollhub" << std::endl;
				disconnect(m_clients.find(fd)->second);
				--ready;
				continue;
			}
			if ((events & (EPOLLHUP | EPOLLERR)) && type == CGI_PIPE)
			{
				std::map<int, Client*>::iterator it = m_client_pipes.find(fd);
				if (it == m_client_pipes.end()) {
					--ready;
					delete (m_events.find(fd)->second);
					m_events.erase(fd);
					continue ;
				}
				if (!receivePipe(*m_client_pipes.at(fd), fd)) {
					Client* client = m_client_pipes.at(fd);
					client->m_cgi_handler.setCgiResponse();
					m_client_pipes.erase(fd);
					deleteCgi(data, fd);
				}
				--ready;
				continue;
			}
			else if(events & (EPOLLIN | EPOLLOUT))
			{
				if (type == LISTENER_SOCK && (events & EPOLLIN))
					acceptClient(m_listeners.find(fd)->second);
				else if (type == CLIENT_SOCK && (events & EPOLLIN)) {
					receiveClient(m_clients.find(fd)->second);
				}
				else if (type == CLIENT_SOCK && (events & EPOLLOUT)) {
					sendClient(m_clients.find(fd)->second);
				}
				else if (type == (CGI_PIPE)) {
					std::map<int, Client*>::iterator it = m_client_pipes.find(fd);
					if (it != m_client_pipes.end())
					{
						receivePipe(*it->second, fd);
					}
				}
			}
			--ready;
		}
	}
}

