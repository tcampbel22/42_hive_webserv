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
	// for (int i = 0 ; i < 2 ; i++)
	// for (auto it : _ip_port_list) //Iterate through host and port pairs (host is first and port is second)
	for (u_long i = 0 ; i < _ip_port_list.size() ; i++)
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
		// std::string newKey = it.first + ":" + std::to_string(it.second);

		settings_vec[i]._fd = serverFd;
		// for (auto it2 : settings_vec)
		// {
		// 	if (it2.getKey() == newKey)
		// 		it2._fd = serverFd;
		// }
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
	// auto it2 = settings.find("127.0.0.1:8081");
	// LocationSettings* locptr = it2->second.getLocationBlock("/");
	// ConfigUtilities::printLocationBlock(*locptr);
	std::signal(SIGINT, signalHandler);
	//std::cout << "Server listening on port " << _port << std::endl;

	 epollFd = epoll_create1(0); //create epoll instance
	 //for (auto it : settings_vec) //iterate through fd vector and add to epoll
	for (u_long i = 0 ; i < settings_vec.size() ; i++)
	 {
		auto it = settings_vec[i];
		_events.events = EPOLLIN;
		fdNode *node = new fdNode;
		node->fd = settings_vec[i]._fd;
		node->serverPtr = &settings_vec[i];
		_events.data.ptr = node;
		
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
			fdNode *nodePtr = static_cast <fdNode*>(_eventsArr[i].data.ptr);
			int	eventFd = nodePtr->fd;
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
					fdNode *node = new fdNode;
					node->fd = _clientSocket;
					node->serverPtr = nodePtr->serverPtr;
					_events.data.ptr = node;
					// _events.data.fd = _clientSocket;
					
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
				int _fd_out = nodePtr->fd;
				if (nodePtr == nullptr)
					 std::cout << "We're fucked!!!\n";

                ssize_t bytesReceived = 0;
                size_t bytes = 1024;
                bool requestComplete = false;
                while (!requestComplete)
                {
                    nodePtr->_clientDataBuffer.resize(nodePtr->_clientDataBuffer.size() + bytes);
                    bytesReceived = recv(_fd_out, &nodePtr->_clientDataBuffer[nodePtr->_clientDataBuffer.size() - bytes], bytes, 0);

                    if (bytesReceived < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) //these are here untill we fix the reading cycle with epoll
                        {
                            break;
                        }
                        else
                        {
                            std::cerr << "Error receiving data: " << strerror(errno) << std::endl;
                            break;
                        }
                    }
                    else if (bytesReceived == 0)
                    {
                        //std::cout << "Client closed the connection." << std::endl;
                        requestComplete = true;
                    }
                    else
                    {
                        //std::cout << "Received " << bytesReceived << " bytes from client." << std::endl;
                        requestComplete = isRequestComplete(nodePtr->_clientDataBuffer, bytesReceived);
                    }
                }
                if (requestComplete)
                {
                    // Once we have the full data, process the request
                    HttpParser::bigSend(nodePtr);  // Send response
                    epoll_ctl(epollFd, EPOLL_CTL_DEL, _fd_out, &_events);  // Remove client socket from epoll
                    close(_fd_out);  // Close the client socket

                    delete nodePtr;  // Clean up the node pointer
                }
                else
                {
					//update the epoll here after an incomplete read.
                    std::cout << "Waiting for more data..." << std::endl;
                }
				//HttpParser::bigSend(_fd_out, nodePtr->serverPtr); // Need to update this with Eromon
				// _events.events = EPOLLIN; 
				// _events.data.fd = _fd_out;
				epoll_ctl(epollFd, EPOLL_CTL_DEL, _fd_out, &_events); //guard later
				close (_fd_out); //needs to be handled in http parsing, client will send whether to close connection or not
				//delete nodePtr;
            }
		}
			fdActivityLoop(current_time);
	}
	std::cout << "outofloop" << std::endl;
	//close (_clientSocket);
	close (epollFd); //Probably not needed
}

// Function to check if the request is fully received (for chunked encoding or complete body)
bool HttpServer::isRequestComplete(const std::vector<char>& data, size_t bytesRead)
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
	bool hasBody = isRequestWithBody(requestStr);
	if (hasBody) {
		size_t complete = getContentLength(requestStr); //with nonchunked body;
		if (complete == bytesRead)
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
