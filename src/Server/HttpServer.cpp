/**********************************************************************************/
/** __          __  _                                                            **/
/** \ \        / / | |                                by:                        **/
/**  \ \  /\  / /__| |__  ___  ___ _ ____   __                                   **/
/**   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / /        Eromon Agbomeirele         **/
/**    \  /\  /  __/ |_) \__ \  __/ |   \ V /         Casimir Lundberg           **/
/**     \/  \/ \___|_.__/|___/\___|_|    \_/          Tim Campbell               **/
/**                                                                              **/
/**                                                                              **/
/**                                W E B S E R V                                 **/
/**********************************************************************************/

#include "HttpServer.hpp"

HttpServer* HttpServer::_instance = nullptr;

HttpServer::HttpServer(std::vector<ServerSettings> vec)
{
	this->_instance = this;
	settings_vec = vec;
	fillHostPortPairs();
	_clientSocket = -1;
	startServer();
}

void HttpServer::startServer()
{
	for (u_long i = 0 ; i < _ip_port_list.size() ; i++)  //Iterate through host and port pairs (host is first and port is second)
	{
		auto it = _ip_port_list[i];
		int serverFd = socket(AF_INET, SOCK_STREAM, 0);
		if (serverFd == -1)
		{
			Logger::log("failed to create socket: " + it.first, ERROR, true);
			continue;
		}
		//store socket addr info
		int optionValue = 1;
		setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
		memset(&_socketInfo, 0, sizeof(_socketInfo));
		_socketInfo.sin_family = AF_INET; //macro for IPV4
		_socketInfo.sin_port = htons(it.second); //converts port number to network byte order
		_socketInfo.sin_addr.s_addr = inet_addr(it.first.c_str()); //converts ip address from string to uint

		//bind socket to port
		if (bind(serverFd, (sockaddr *)&_socketInfo, sizeof(_socketInfo)) < 0)
		{
			Logger::log("failed to bind socket: " + it.first, INFO, true);
			close(serverFd);
			continue;
		}
		if (listen(serverFd, SOMAXCONN) < 0)
		{
			Logger::log("failed to listen", INFO, true);
			close(serverFd);
			continue;
		}
		settings_vec[i]._fd = serverFd;
		_server_fds.push_back(serverFd); //add fd to vector to use later in listening function
		Logger::log("Listening on host: " + (std::string)it.first + " Port: " + std::to_string(it.second), INFO, true);
		_connections++;
	}
}

bool	HttpServer::safeEpollCtl(e_poll event_type, fdNode* node, e_ctl ctl, int fd)
{
	if (!node)
		return false;
	epoll_event event;
	if (event_type == E_IN)
		event.events = EPOLLIN;
	else if (event_type == E_OUT)
		event.events = EPOLLOUT;
	else
		event.events = 0;
	event.data.ptr = node;
	if (fd == -1)
		fd = node->fd;
	if (ctl == DEL)
	{
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, nullptr) == -1)
		{
			Logger::log("epoll-ctl: Failed to delete from epoll", ERROR, false);
			client_nodes.erase(node->fd);
			_fd_activity_map.erase(node->fd);
			close(node->fd);
			return false ;
		}
	}
	else if (ctl == ADD)
	{
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event) == -1)
		{
			Logger::log("epoll-ctl: Failed to add event to epoll", ERROR, false);
			client_nodes.erase(node->fd);
			_fd_activity_map.erase(node->fd);
			close(node->fd);
			return false ;
		}
	}
	else
	{
		if (epoll_ctl(epollFd, EPOLL_CTL_MOD, fd, &event) == -1)
		{
			Logger::log("epoll-ctl: Failed to mod event to epoll", ERROR, false);
			client_nodes.erase(node->fd);
			_fd_activity_map.erase(node->fd);
			close(node->fd);
			return false ;
		}
	}
	return true;	
}

void HttpServer::startListening()
{
	std::signal(SIGINT, signalHandler);
	std::signal(SIGPIPE, SIG_IGN);
	
	addServerToEpoll();
	while (true)
	{
		numEvents = epoll_wait(epollFd, _eventsArr, MAX_EVENTS, 0);
		if (numEvents < 0)
		{
			Logger::log("epoll_wait failed", ERROR, false);
			break ;
		}
		time_t current_time = std::time(nullptr);
		for (int i = 0; i < numEvents; i++)
		{
			fdNode *nodePtr = static_cast <fdNode*>(_eventsArr[i].data.ptr);  //retrieving current serversettings and client fd
			if (std::find(_server_fds.begin(), _server_fds.end(), nodePtr->fd) != _server_fds.end()) //eventFd is a server_socket meaning a new request is incoming
				acceptNewClient(nodePtr->serverPtr, nodePtr->fd, current_time); //Send in server socket settings to create new node
			else if (nodePtr && !checkSystemMemory(nodePtr) && _eventsArr[i].events & EPOLLIN) //client socket has data to read from
			{
				if (!handle_read(nodePtr))
					continue;
            }
			else if (nodePtr && !checkSystemMemory(nodePtr) && _eventsArr[i].events & EPOLLOUT && nodePtr->_readyToSend)
			{
				if (!handle_write(nodePtr))
					continue;
			}
		fdActivityLoop(current_time);
		}
	}
	close(epollFd);
}

void	HttpServer::addServerToEpoll()
{
	epollFd = epoll_create1(0); //create epoll instance
	if (epollFd < 0)
	{
		Logger::log("epoll_create: create failed", ERROR, true);
		this->~HttpServer();
		return ;
	}
	for (u_long i = 0 ; i < settings_vec.size() ; i++)  //iterate through fd vector and add to epoll
	 {
		epoll_event event;
		
		auto it = settings_vec[i];
		std::shared_ptr<fdNode> server_node = std::make_shared<fdNode>();
		server_node->fd = settings_vec[i]._fd;
		server_node->serverPtr = &settings_vec[i];
		server_nodes.push_back(server_node);
		
		setNonBlocking(server_node->fd);
		event.data.ptr = server_node.get();
		event.events = EPOLLIN | EPOLLOUT;
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, settings_vec[i]._fd, &event) == -1)		
		{
			Logger::log("epoll_ctl: failed to add to epoll", ERROR, false);
			continue; 
		}
	 }
}

void	HttpServer::acceptNewClient(ServerSettings* settingsPtr, int serverFd, time_t current_time)
{
	socklen_t _sockLen = sizeof(_socketInfo);
	memset(&_socketInfo, 0, sizeof(_socketInfo));

	if (_connections < MAX_CONNECTIONS)
	{
		_clientSocket = accept(serverFd, (sockaddr *)&_socketInfo, &_sockLen);
		if (_clientSocket < 0) 
			Logger::log("accept: accept failed", ERROR, false);
		else
		{
			Logger::log("New client connected: " + std::to_string(_clientSocket), INFO, false);
			setNonBlocking(_clientSocket);
			createClientNode(settingsPtr);
			_fd_activity_map[_clientSocket] = current_time;
		}
	}
	else
		Logger::log("max connections reached", ERROR, false);
}

std::string getBoundary(std::string requestString) {
	size_t pos = requestString.find("WebKitFormBoundary");
	std::string boundaryCode = requestString.substr(pos, requestString.find_first_of("\r\n", pos) - pos);
	boundaryCode.append("--");
	return boundaryCode;
}

HttpServer::~HttpServer()
{
	close(epollFd);
	for (const auto& it : pipe_vec)
	{
		close(it.first);
		close(it.second);
	}
	_ip_port_list.clear();
	for (auto it = settings_vec.begin(); it != settings_vec.end(); it++)
		close(it->_fd);
	for (auto it : client_nodes)
	{
		close(it.first);
		it.second.reset();
	}
	for (auto it : server_nodes)
		close(it->fd);
	pipe_vec.clear();
	client_nodes.clear();
	settings_vec.clear();
	settings_vec.shrink_to_fit();
	_server_fds.clear();
}
