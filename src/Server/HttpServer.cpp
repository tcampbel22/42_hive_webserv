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

void testSend(int fd)
{
	Response newResponse;
	newResponse.setResponseCode(200);
	newResponse.setContentType("text/html");
	newResponse.setContentLength(137);
	newResponse.setCloseConnection(false);
	newResponse.set_body("<!DOCTYPE html>\n<html>\n<head>\n    <title>Simple C++ Web Server</title>\n</head>\n<body>\n    <h1>Hello from a C++ web server!</h1>\n</body>\n</html>\n");
	newResponse.sendResponse(fd);
}

void HttpServer::signalHandler(int signal)
{
	(void)signal;
	std::cout <<  "\nExit signal received, server shutting down.. " << std::endl;
	_instance->closeServer();
	exit(0);
}

void HttpServer::startServer()
{
	//make socket
	for (auto it : _ip_port_list) //Iterate through host and port pairs (host is first and port is second)
	{
		int serverFd = socket(AF_INET, SOCK_STREAM, 0);
		if (serverFd == -1)
		{
			ft_perror("failed to create socket: " + it.first);
			continue;
		}
		//std::cout << _serverFd << std::endl;
		
		//store socket addr info
		int optionValue = 1;
		setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
		_socketInfo.sin_family = AF_INET; //macro for IPV4
		_socketInfo.sin_port = htons(it.second); //converts port number to network byte order
		_socketInfo.sin_addr.s_addr = inet_addr(it.first.c_str()); //converts ip address from string to uint

		//bind socket to port
		if (bind(serverFd, (sockaddr *)&_socketInfo, sizeof(_socketInfo)) < 0)
		{
			ft_perror("failed to bind socket: " + it.first);
			close(serverFd);
			continue;
		}
		if (listen(serverFd, 5) < 0)
		{
			ft_perror("failed to listen");
			close(serverFd);
			continue;
		}
		_server_fds.push_back(serverFd); //add fd to vector to use later in listening function
		std::cout << "Listening on host: " << it.first << " Port: " << it.second << '\n';
	}
}

void	setNonBlocking(int socket)
{
	int	flag = fcntl(socket, F_GETFL, 0); //retrieves flags/settings from socket
	fcntl(socket, F_SETFL, flag | O_NONBLOCK); //Sets socket to be nonblocking
}

void HttpServer::startListening()
{

	std::signal(SIGINT, signalHandler);
	//std::cout << "Server listening on port " << _port << std::endl;

	 epollFd = epoll_create1(0); //create epoll instance
	 for (int fd : _server_fds) //iterate through fd vector and add to epoll
	 {
		_events.data.fd = fd;
		_events.events = EPOLLIN;
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &_events) == -1)
		{
			ft_perror("Failed to add to epoll");
			continue; 
		}
	 }
	
	while (true)
	{
		numEvents = epoll_wait(epollFd, _eventsArr, MAX_EVENTS, 0);
		if (numEvents < 0){
			std::cout << "Epoll wait failed\n";
			break ;}
		time_t current_time = std::time(nullptr);
		for (int i = 0; i < numEvents; i++)
		{
			int	eventFd = _eventsArr[i].data.fd;
			if (std::find(_server_fds.begin(), _server_fds.end(), eventFd) != _server_fds.end())
			{
					socklen_t _sockLen = sizeof(_socketInfo);
					_clientSocket = accept(eventFd, (sockaddr *)&_socketInfo, &_sockLen);
					if (_clientSocket < 0) 
					{
						std::cerr << "accept failed\n" << strerror(errno) << '\n';
						continue;
					}
					std::cout << "New client connected: " << _clientSocket << std::endl;
					setNonBlocking(_clientSocket);
					
					_events.events = EPOLLIN | EPOLLOUT;
					_events.data.fd = _clientSocket;
					
					if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _clientSocket, &_events) == -1)
					{
						ft_perror("failed to add fd to epoll");
						close(_clientSocket);
						continue;
					}
					_fd_activity_map[_clientSocket] = current_time;
			}
			else if (_eventsArr[i].events & EPOLLIN)
			{	
				int _fd_out = _eventsArr[i].data.fd;
				//testSend(_fd_out);
				
				//ServerHandler handle_request(_fd_out, );
				HttpParser::bigSend(_fd_out, settings); // Need to update this with Eromon
				// _events.events = EPOLLIN; 
				// _events.data.fd = _fd_out;
				epoll_ctl(epollFd, EPOLL_CTL_DEL, _fd_out, &_events); //guard later
				close (_fd_out); //needs to be handled in http parsing, client will send whether to close connection or not
			}
			fdActivityLoop(current_time);
		}
	}
	std::cout << "outofloop" << std::endl;
	//close (_clientSocket);
	close (epollFd); //Probably not needed
}
// If the client has been inactive for too long, close the socket
void HttpServer::fdActivityLoop(const time_t current_time) {
	for (auto it = _fd_activity_map.begin(); it != _fd_activity_map.end();) {
            if (current_time - it->second > TIME_OUT_PERIOD) {
                std::cout << "Timeout: Closing client socket " << it->first << std::endl;
                close(it->first);
                epoll_ctl(epollFd, EPOLL_CTL_DEL, it->first, &_events);
                it = _fd_activity_map.erase(it);
            } else {
                ++it;
            }
        }
}


void HttpServer::closeServer()
{
	close(epollFd);
	for (auto it = _server_fds.begin(); it != _server_fds.end(); it++)
		close(*it);
}

HttpServer::~HttpServer()
{
	closeServer();

};
// HttpServer::HttpServer(std::shared_ptr<ServerSettings> _settings)
// {
// 	this->_instance = this;
// 	settings = _settings;
// 	_port = settings->getPort();
// 	_ipAddress = settings->getHost();
// 	_clientSocket = -1;
// 	startServer();
// };

void	HttpServer::fillHostPortPairs()
{
	for (auto& pair : settings)
	{
		_ip_port_list.push_back( {pair.second.getHost(), pair.second.getPort()} );
	}
}


HttpServer::HttpServer(std::unordered_map<std::string, ServerSettings>& _settings)
{
	this->_instance = this;
	settings = _settings;
	fillHostPortPairs();
	// _port = settings->getPort();
	// _ipAddress = settings->getHost();
	_clientSocket = -1;
	startServer();
}
