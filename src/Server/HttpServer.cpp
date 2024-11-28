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
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
		return ; //change later
	//std::cout << _serverFd << std::endl;
	
	//store socket addr info
	int optionValue = 1;
	setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
	_socketInfo.sin_family = AF_INET; //macro for IPV4
	_socketInfo.sin_port = htons(_port); //converts port number to network byte order
	_socketInfo.sin_addr.s_addr = inet_addr(_ipAddress.c_str()); //converts ip address from string to uint

	//bind socket to port
	if (bind(_serverFd, (sockaddr *)&_socketInfo, sizeof(_socketInfo)) < 0)
		return ; //change later and remember to close all fds
	if (listen(_serverFd, 5) < 0)
	{
		std::cout << "listen failed\n";
		return ; //change later, also close all fds
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

	std::cout << "Server listening on " << settings->getPort() << std::endl;

	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: 137\n\n<!DOCTYPE html>\n<html>\n<head>\n    <title>Simple C++ Web Server</title>\n</head>\n<body>\n    <h1>Hello from a C++ web server!</h1>\n</body>\n</html>\n";

	std::string response2 = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: 137\n\n<!DOCTYPE html>\n<html>\n<head>\n    <title>Simple C++ Web Server</title>\n</head>\n<body>\n    <h1>Hello from a shitty C++ web server!</h1>\n</body>\n</html>\n";

	 epollFd = epoll_create1(0);
	_events.data.fd = _serverFd;
	 _events.events = EPOLLIN;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, _serverFd, &_events); //maybe protect
	
	while (true)
	{
		numEvents = epoll_wait(epollFd, _eventsArr, MAX_EVENTS, 0);
		if (numEvents < 0){
			std::cout << "Epoll wait failed\n";
			break ;}
			
		for (int i = 0; i < numEvents; i++)
		{
			if (_eventsArr[i].data.fd == _serverFd)
			{
				socklen_t _sockLen = sizeof(_socketInfo);
				_clientSocket = accept(_serverFd, (sockaddr *)&_socketInfo, &_sockLen);
				if (_clientSocket < 0) 
				{
					std::cerr << "accept failed\n" << strerror(errno) << '\n';
					continue;
				}
				std::cout << "New client connected: " << _clientSocket << std::endl;
				setNonBlocking(_clientSocket);
				
				_events.events = EPOLLIN | EPOLLOUT;
                _events.data.fd = _clientSocket;
				
				epoll_ctl(epollFd, EPOLL_CTL_ADD, _clientSocket, &_events); //guard later
			}
			else if (_eventsArr[i].events & EPOLLIN)
			{	
				int _fd_out = _eventsArr[i].data.fd;
				//testSend(_fd_out);

				//ServerHandler handle_request(_fd_out, );
				HttpParser::bigSend(_fd_out, this->settings);
				// _events.events = EPOLLIN; 
                // _events.data.fd = _fd_out;
				epoll_ctl(epollFd, EPOLL_CTL_DEL, _fd_out, &_events); //guard later
				close (_fd_out); //needs to be handled in http parsing, client will send whether to close connection or not
			
			}
		}
	}
	std::cout << "outofloop" << std::endl;
	//close (_clientSocket);
	close (epollFd);
}

void HttpServer::closeServer()
{
	close(epollFd);
	close(_serverFd);
}

HttpServer::~HttpServer()
{
	closeServer();

};
HttpServer::HttpServer(std::shared_ptr<ServerSettings> _settings)
{
	this->_instance = this;
	settings = _settings;
	_port = settings->getPort();
	_ipAddress = settings->getHost();
	_clientSocket = -1;
	startServer();
};
