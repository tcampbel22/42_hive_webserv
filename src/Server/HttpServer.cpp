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
#include "../Logger/Logger.hpp"

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
	Logger::closeLogger();
	exit(0);
}

void HttpServer::startServer()
{
	for (u_long i = 0 ; i < _ip_port_list.size() ; i++)  //Iterate through host and port pairs (host is first and port is second)
	{
		auto it = _ip_port_list[i];
		int serverFd = socket(AF_INET, SOCK_STREAM, 0);
		if (serverFd == -1)
		{
			ft_perror("failed to create socket: " + it.first);
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
		settings_vec[i]._fd = serverFd;
		_server_fds.push_back(serverFd); //add fd to vector to use later in listening function
		std::cout << "Listening on host: " << it.first << " Port: " << it.second << '\n';
	}
}

void	setNonBlocking(int socket)
{
	int	flag = fcntl(socket, F_GETFL, 0); //retrieves flags/settings from socket
	if (flag < 0)
		std::cout << "GETFL failed\n";
	if (fcntl(socket, F_SETFL, flag | O_NONBLOCK) < 0) //Sets socket to be nonblocking
		std::cout << "SETFL failed\n"; 
}

void HttpServer::startListening()
{
	std::signal(SIGINT, signalHandler);
	std::signal(SIGPIPE, SIG_IGN);
	
	addServerToEpoll();
	while (true)
	{
		numEvents = epoll_wait(epollFd, _eventsArr, MAX_EVENTS, 0);
		if (numEvents < 0){
			std::cout << "Epoll wait failed\n";
			break ;}
		time_t current_time = std::time(nullptr);
		for (int i = 0; i < numEvents; i++)
		{
			fdNode *nodePtr = static_cast <fdNode*>(_eventsArr[i].data.ptr);  //retrieving current serversettings and client fd
			int	eventFd = nodePtr->fd;
			if (std::find(_server_fds.begin(), _server_fds.end(), eventFd) != _server_fds.end()) //eventFd is a server_socket meaning a new request is incoming
				acceptNewClient(nodePtr, eventFd, current_time);
			else if (_eventsArr[i].events & EPOLLIN) //client socket has data to read from
			{	
				int _fd_out = nodePtr->fd;

                ssize_t bytesReceived = 0;
                ssize_t bytes = 1024;
				bool requestComplete = false;
                while (!requestComplete)
                {
                    nodePtr->_clientDataBuffer.resize(nodePtr->_clientDataBuffer.size() + bytes);
                    bytesReceived = recv(_fd_out, &nodePtr->_clientDataBuffer[nodePtr->_clientDataBuffer.size() - bytes], bytes, 0);

					if (bytesReceived < bytes) {
						if (bytesReceived < 0)
							bytesReceived = 0;
						nodePtr->_clientDataBuffer.resize(nodePtr->_clientDataBuffer.size() - (bytes - bytesReceived));
					}

					if (bytesReceived < 0)
                    {
						// if (errno == EAGAIN || errno == EWOULDBLOCK) {
							requestComplete = true; //this is for the tester, tester sends stuff in a weird format, need this to go forward
							break;
                    }
                    else if (bytesReceived == 0)
                    {
                        std::cout << "Client closed the connection." << std::endl;
						Logger::log("Client closed the connection.", INFO);
                        requestComplete = true;
						_clientClosedConn = true;
                    }
                    else
                    {
                        // std::cout << "Received " << bytesReceived << " bytes from client." << std::endl;
                        requestComplete = isRequestComplete(nodePtr->_clientDataBuffer, bytesReceived);
                    }
                }
                if (requestComplete)
                {
					//nodePtr->_clientDataBuffer.resize(nodePtr->_clientDataBuffer.size() - (bytes - bytesReceived));
                    // Once we have the full data, process the request
                    if (HttpParser::bigSend(nodePtr, epollFd, _events) || _clientClosedConn == true)
					{
						epoll_ctl(epollFd, EPOLL_CTL_DEL, _fd_out, &_events);  // Remove client socket from epoll
						client_nodes.erase(_fd_out); //delete node pointer
						nodePtr->_clientDataBuffer.clear();
						delete nodePtr;
						close(_fd_out);  // Close the client socket
						_clientClosedConn = false;
					}
					else
						nodePtr->_clientDataBuffer.clear();
                }
                else
                {
					_events.events = EPOLLIN;  //update the epoll here after an incomplete read.
                    std::cout << "Waiting for more data..." << std::endl;
					break ;
                }
            }
		}
		fdActivityLoop(current_time);
	}
	close(epollFd);
}

void	HttpServer::addServerToEpoll()
{
	// epollFd = epoll_create1(0); //create epoll instance
	for (u_long i = 0 ; i < settings_vec.size() ; i++)  //iterate through fd vector and add to epoll
	 {
		auto it = settings_vec[i];
		_events.events = EPOLLIN | EPOLLET;
		fdNode* server_node = new fdNode;
		server_node->fd = settings_vec[i]._fd;
		server_node->serverPtr = &settings_vec[i];
		_events.data.ptr = server_node;
		server_nodes.push_back(server_node);
		setNonBlocking(server_node->fd);
		
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, settings_vec[i]._fd, &_events) == -1)		
		{
			ft_perror("Failed to add to epoll");
			Logger::log("Failed to add to epoll", ERROR);
			continue; 
		}
	 }
}

void	HttpServer::acceptNewClient(fdNode* nodePtr, int eventFd, time_t current_time)
{
	socklen_t _sockLen = sizeof(_socketInfo);
	memset(&_socketInfo, 0, sizeof(_socketInfo));
	// _socketInfo.sin_family = AF_INET; //macro for IPV4
	// _socketInfo.sin_port = htons(8081); //converts port number to network byte order
	// _socketInfo.sin_addr.s_addr = inet_addr("127.0.0.1"); //converts ip address from string to uint
	_clientSocket = accept(eventFd, (sockaddr *)&_socketInfo, &_sockLen);
	if (_clientSocket < 0) 
	{
		std::cerr << "accept failed\n" << strerror(errno) << '\n';
		Logger::log("accept failed\n", ERROR);
	}
	Logger::log("New client connected: " + std::to_string(_clientSocket), INFO);
	std::cout << "New client connected: " << _clientSocket << std::endl;
	setNonBlocking(_clientSocket);
	
	_events.events = EPOLLIN | EPOLLOUT;
	fdNode *client_node = new fdNode;
	client_node->fd = _clientSocket;
	client_nodes[_clientSocket] = client_node;
	client_node->serverPtr = nodePtr->serverPtr;
	_events.data.ptr = client_node;
	
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _clientSocket, &_events) == -1)
	{
		ft_perror("failed to add fd to epoll");
		Logger::log("Failed to add to epoll", ERROR);
		close(_clientSocket);
		delete client_node;
	}
	_fd_activity_map[_clientSocket] = current_time;
}


// Function to check if the request is fully received (for chunked encoding or complete body)
bool HttpServer::isRequestComplete(const std::vector<char>& data, ssize_t bytesReceived)
{
    std::string requestStr(data.begin(), data.end());
	bool isChunked = isChunkedTransferEncoding(requestStr);
	if (isChunked) {
		if (requestStr.find("0\r\n\r\n") != std::string::npos) {  // End of chunked data
        	return true;
   		}
		else
			return false;
	}
	(void) bytesReceived;
	bool hasBody = isRequestWithBody(requestStr);
	if (hasBody) {
		// size_t complete = getContentLength(requestStr); //with nonchunked body;
		// if (complete > 0 && bytesReceived != 1024)
		// if (bytesReceived != 1024)
		int test = 0;
		test = requestStr.find("\r\n\r\n");
		if (requestStr.find("\r\n\r\n", test + 4) != std::string::npos)
			return true;
		else
			return false;
	}
	if (!isChunked && !hasBody) {
		if (requestStr.find("\r\n\r\n") != std::string::npos) {  // End of nonBody data 
			return true;
   		}
		else
			return false;
	}
    return false;
}
size_t HttpServer::getContentLength(const std::string& requestStr)
{
    size_t pos = requestStr.find("Content-Length: ");
    if (pos != std::string::npos)
    {
        size_t start = pos + 15;
        size_t end = requestStr.find("\r\n", start);
        std::string lengthStr = requestStr.substr(start, end - start);
        return std::stoi(lengthStr);
    }
    return 0;
}

bool HttpServer::isRequestWithBody(std::string requestStr) { return requestStr.find("Content-Length:") != std::string::npos; }

bool HttpServer::isChunkedTransferEncoding(const std::string& requestStr) { return requestStr.find("Transfer-Encoding: chunked") != std::string::npos; }

// If the client has been inactive for too long, close the socket
void HttpServer::fdActivityLoop(const time_t current_time)
{
	for (auto it = _fd_activity_map.begin(); it != _fd_activity_map.end();) {
            if (current_time - it->second > TIME_OUT_PERIOD) {
                std::cout << "Timeout: Closing client socket " << it->first << std::endl;
				Logger::log("Timeout: Closing client socket " + std::to_string(it->first), INFO);
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
	for (auto it : server_nodes)
		delete it;
	for (auto it : client_nodes)
	{
		delete it.second;
	}
	settings_vec.clear();
	settings_vec.shrink_to_fit();
}

HttpServer::~HttpServer()
{
	closeServer();

};

void	HttpServer::fillHostPortPairs()
{
	for (auto& it : settings_vec)
	{
		_ip_port_list.push_back( {it.getHost(), it.getPort()} );
	}
}


HttpServer::HttpServer(std::vector<ServerSettings>& vec)
{
	this->_instance = this;
	settings_vec = vec;
	fillHostPortPairs();
	_clientSocket = -1;
	startServer();
}
