/************************************************/
/** __          __  _                          **/
/** \ \        / / | |                         **/
/**  \ \  /\  / /__| |__  ___  ___ _ ____   __ **/
/**   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / / **/
/**    \  /\  /  __/ |_) \__ \  __/ |   \ V /  **/
/**     \/  \/ \___|_.__/|___/\___|_|    \_/   **/
/**                                            **/
/**                                            **/
/**             W E B S E R V                  **/
/************************************************/

#include "../include/HttpServer.hpp"


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

void HttpServer::startServer()
{
	//make socket
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
		return ; //change later
	//std::cout << _serverFd << std::endl;
	
	//store socket addr info
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
	std::cout << "Server listening on port " << _port << std::endl;
	
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
			else if (_eventsArr[i].events & EPOLLOUT)
			{	
				int _fd_out = _eventsArr[i].data.fd;
				//testSend(_fd_out);
				ServerHandler handle_request(_fd_out);
				//HttpParser::bigSend(_fd_out, "./assets/response.html");
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
	close(_serverFd);
}

HttpServer::~HttpServer()
{
	closeServer();
};
HttpServer::HttpServer(const std::string _ip, uint _newPort)
{
	_port = _newPort;
	_ipAddress = _ip;
	_clientSocket = -1;
	startServer();
	
};
