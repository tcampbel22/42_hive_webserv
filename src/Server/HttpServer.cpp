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
#include <string>

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
	for (int i = 0 ; i < 2 ; i++)
	// for (auto it : _ip_port_list) //Iterate through host and port pairs (host is first and port is second)
	{
		auto it = _ip_port_list[i];
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

		//ADD to serverblock instance

		std::cout << serverFd << " server fd\n"; 

		if (listen(serverFd, 5) < 0)
		{
			ft_perror("failed to listen");
			close(serverFd);
			continue;
		}
		// std::string newKey = it.first + ":" + std::to_string(it.second);

		settings_vec[i]._fd = serverFd;
		// for (auto it2 : settings_vec)
		// {
		// 	if (it2.getKey() == newKey)
		// 		it2._fd = serverFd;
		// }
		_server_fds.push_back(&serverFd); //add fd to vector to use later in listening function
		std::cout << "Listening on host: " << it.first << " Port: " << it.second << '\n';
	}
	std::cout << settings_vec[0].getPort() << std::endl;
	std::cout << settings_vec[1].getPort() << std::endl;
}

void	setNonBlocking(int socket)
{
	int	flag = fcntl(socket, F_GETFL, 0); //retrieves flags/settings from socket
	fcntl(socket, F_SETFL, flag | O_NONBLOCK); //Sets socket to be nonblocking
}

void HttpServer::startListening()
{
	// auto it2 = settings.find("127.0.0.1:8081");
	// LocationSettings* locptr = it2->second.getLocationBlock("/");
	// ConfigUtilities::printLocationBlock(*locptr);
	std::signal(SIGINT, signalHandler);
	//std::cout << "Server listening on port " << _port << std::endl;

	 epollFd = epoll_create1(0); //create epoll instance
	 //for (auto it : settings_vec) //iterate through fd vector and add to epoll
	for (int i = 0 ; i < 2 ; i++)
	 {
		auto it = settings_vec[i];
		_events.events = EPOLLIN;
		// _events.data.ptr = &it;
		// std::cout << settings_vec[i].getPort() << std::endl;
		_events.data.ptr = &settings_vec[i];
		// for (auto it : settings)
		// {
		// 	if (it.second.fdPtr == &fd)
		// 		_events.data.ptr = &it.second;
		// }
		
		std::cout << it._fd << '\n';
		// if (epoll_ctl(epollFd, EPOLL_CTL_ADD, it._fd, &_events) == -1)
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, settings_vec[i]._fd, &_events) == -1)		
		{
			ft_perror("Failed to add to epol11l");
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
			ServerSettings *serverPtr = static_cast <ServerSettings*>(_eventsArr[i].data.ptr);
			int	eventFd = serverPtr->_fd;
			if (std::find(_server_fds.begin(), _server_fds.end(), &eventFd) != _server_fds.end())
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

				// ServerSettings *serverPtr = static_cast <ServerSettings*>(_eventsArr[i].data.ptr);
				int _fd_out = serverPtr->_fd;

				//testSend(_fd_out);
				int hdd = serverPtr->getPort();
				std::cout << hdd << std::endl;
				if (serverPtr == nullptr)
					 std::cout << "We're fucked!!!\n";

				//ServerHandler handle_request(_fd_out, );
				HttpParser::bigSend(_fd_out, serverPtr); // Need to update this with Eromon
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
	for (auto it = settings_vec.begin(); it != settings_vec.end(); it++)
		close(it->_fd); 

}

HttpServer::~HttpServer()
{
	closeServer();

};

void	HttpServer::fillHostPortPairs()
{
	for (auto& pair : settings)
	{
		_ip_port_list.push_back( {pair.second.getHost(), pair.second.getPort()} );
	}
}


HttpServer::HttpServer(std::unordered_map<std::string, ServerSettings>& _settings, std::vector<ServerSettings>& vec)
{
	this->_instance = this;
	settings = _settings;
	settings_vec = vec;
	fillHostPortPairs();
	_clientSocket = -1;
	startServer();
}
