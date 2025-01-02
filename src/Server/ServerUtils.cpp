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

void HttpServer::signalHandler(int signal)
{
	(void)signal;
	Logger::log("\nExit signal received, server shutting down.. ", INFO, true);
	_instance->closeServer();
	Logger::closeLogger();
	exit(0);
}

void	HttpServer::fillHostPortPairs()
{
	for (auto& it : settings_vec)
	{
		_ip_port_list.push_back( {it.getHost(), it.getPort()} );
	}
}

void	HttpServer::setNonBlocking(int socket)
{
	int	flag = fcntl(socket, F_GETFL, 0); //retrieves flags/settings from socket
	if (flag < 0)
		Logger::log("GETFL failed", ERROR, true);
	if (fcntl(socket, F_SETFL, flag | O_NONBLOCK) < 0) //Sets socket to be nonblocking
		Logger::log("SETFL failed", ERROR, true); 
}

bool HttpServer::isNonBlockingSocket(int fd) 
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return false;
    }
    return (flags & O_NONBLOCK) != 0;
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
	for (auto it = _fd_activity_map.begin(); it != _fd_activity_map.end();) 
	{
		if (current_time - it->second > TIME_OUT_PERIOD) 
		{
			Logger::log("timeout: closing client socket " + std::to_string(it->first), INFO, true);
			auto node = client_nodes.find(it->first);
			it = _fd_activity_map.erase(it);
			cleanUpFds(node->second);
        } 
		else 
			++it;
    }
}

void	HttpServer::cleanUpFds(fdNode *nodePtr)
{
	epoll_ctl(epollFd, EPOLL_CTL_DEL, nodePtr->fd, &_events);  // Remove client socket from epoll
	client_nodes.erase(nodePtr->fd); //delete fd from fd map
	_fd_activity_map.erase(nodePtr->fd);
	nodePtr->_clientDataBuffer.clear(); //empty data buffer read from client
	close(nodePtr->fd);  // Close the client socket
	delete nodePtr;
	_clientClosedConn = false;
}

void	HttpServer::createClientNode(fdNode* nodePtr)
{
	fdNode *client_node = new fdNode;
	client_node->fd = _clientSocket;
	client_nodes[_clientSocket] = client_node;
	client_node->serverPtr = nodePtr->serverPtr;
	_events.data.ptr = client_node;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _clientSocket, &_events) == -1)
	{
		Logger::log("Failed to add to epoll", ERROR, false);
		close(_clientSocket);
		delete client_node;
	}
}