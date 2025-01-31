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

bool	HttpServer::handle_read(std::shared_ptr<fdNode> nodePtr)
{
	readRequest(nodePtr);
	if (requestComplete)
	{
		if (nodePtr->_clientDataBuffer.empty()) //Either recv error or client closes connection or both
			killNode(nodePtr);
		else
		{
			if (!nodePtr)
				return false;
			nodePtr->_readyToSend = true;
			safeEpollCtl(E_OUT, nodePtr, MOD, -1); //Modify epoll event to epollin, kill node on failure
		}
		return true;
	}
	else
	{
		Logger::log("Waiting for more data...", INFO, false);
		return true;
	}
}

//Read data from client stream
void	HttpServer::readRequest(std::shared_ptr<fdNode>nodePtr)
{
	int _fd_out = nodePtr->fd;

	ssize_t bytesReceived = 0;
	ssize_t bytes = 1024;
	requestComplete = false;

		nodePtr->_clientDataBuffer.resize(nodePtr->_clientDataBuffer.size() + bytes); //resize buffer to current size + amount of bytes to be read
		bytesReceived = recv(_fd_out, &nodePtr->_clientDataBuffer[nodePtr->_clientDataBuffer.size() - bytes], bytes, 0); //Add bytes read to buffer starting from where last read
		Logger::log("Bytes recieved: " + std::to_string(bytesReceived), INFO, false);
		if (nodePtr->_clientDataBuffer.size() >= MAX_HEADER_SIZE) // Check if current buffer exceeds max header size
		{
			if (!nodePtr->headerCorrect && nodePtr->_clientDataBuffer.size() >= MAX_HEADER_SIZE) // Check if header exceeds max header size
			{
				validateHeaders(nodePtr->_clientDataBuffer, &nodePtr->_error); // Check if header has finshed, if not throw 431 error
				if (nodePtr->_error != 0) 
				{
					requestComplete = true;
					nodePtr->headerCorrect = true; // Why set it to true?
					_clientClosedConn = true;
					return ;
				}
				nodePtr->headerCorrect = true;
			}
		}
		if (bytesReceived < bytes)  // Resize buffer if amount of bytes read is less than the read amount, check if request is complete
		{
			int temp = bytesReceived;
			if (bytesReceived < 0) //Prevent overflow if recv returns -1
				temp = 0;
			nodePtr->_clientDataBuffer.resize(nodePtr->_clientDataBuffer.size() - (bytes - temp));
			requestComplete = isRequestComplete(nodePtr->_clientDataBuffer, nodePtr->_clientDataBuffer.size(), nodePtr);
		}
		if (bytesReceived < 0) // Check is recv returns -1, check if request is complete
		{
			if (isNonBlockingSocket(nodePtr->fd)) //check if there is an error with recv
			{
				Logger::log("recv: failed to read, better check ERRNO :/", ERROR, false);
				requestComplete = isRequestComplete(nodePtr->_clientDataBuffer, nodePtr->_clientDataBuffer.size(), nodePtr);
			}
		}
		else if (bytesReceived == 0) //read is successful and client closes connection
		{
			Logger::log("Client closed the connection.", INFO, false);
			requestComplete = true;
			_clientClosedConn = true;
		}
		else
			requestComplete = isRequestComplete(nodePtr->_clientDataBuffer, nodePtr->_clientDataBuffer.size(), nodePtr);
}

bool HttpServer::handle_write(std::shared_ptr<fdNode> nodePtr)
{
	if (nodePtr && nodePtr->cgiStarted == true) //if request is CGI amd node still in scope
	{	
		if (HttpServer::checkCGI(nodePtr) == 1) // monitor cgi child process
		{
			if (HttpParser::bigSend(nodePtr, *_instance) || _clientClosedConn == true)
			{ 
				killNode(nodePtr);
				return false;
			}
			else
			{
				if (!resetCGI(nodePtr))
					return false;
			}
		}
	}
	else if (HttpParser::bigSend(nodePtr, *_instance) || _clientClosedConn == true) // Once we have the full data, process the request
	{
		killNode(nodePtr);
	}
	else if (nodePtr->cgiStarted == false)
	{
		if (!safeEpollCtl(E_IN, nodePtr, MOD, -1))
		{
			Logger::log("handle-write: failed to mod epoll", ERROR, false);
			return false;
		}
		resetNode(nodePtr);
	}
	// if (nodePtr)
	// {
	// 	nodePtr->headerCorrect = false;
	// 	nodePtr->_error = 0;
	// }
	return true;
}

// Function to check if the request is fully received (for chunked encoding or complete body)
bool HttpServer::isRequestComplete(const std::vector<char>& data, ssize_t bytesReceived, std::shared_ptr<fdNode> node)
{
    std::string requestStr(data.begin(), data.end()); //Convert buffer to string
	// std::cout << requestStr << std::endl;
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
		int complete = getContentLength(requestStr);
		if (complete < 0 || !validateContentLength(node, complete))
			return true;
		size_t test = requestStr.find("\r\n\r\n") + 4;
		if ((requestStr.find("\r\n\r\n", test) != std::string::npos) || (bytesReceived - test == (size_t)complete))
			return true;
		else
			return false;
	}
	if (!isChunked && !hasBody) 
	{
		if (requestStr.find("\r\n\r\n") != std::string::npos)  // End of nonBody data
			return true;
		else
			return false;
	}
    return false;
}

int HttpServer::checkCGI(std::shared_ptr<fdNode>requestNode)
{
	char 	buffer[1024]; //CGI buffer
	ssize_t bytesRead = 0; //for CGI reading

	int status;
	pid_t result = waitpid(requestNode->pid, &status, WNOHANG);
	if (result == requestNode->pid)
	{
		if (WIFEXITED(status))
		{
			if(WEXITSTATUS(status))
			{
				Logger::setErrorAndLog(&requestNode->CGIError, 502, "child process failed");
				requestNode->CGIReady = true;
				safeEpollCtl(EMPTY, requestNode, DEL, requestNode->pipe_fds[READ_END]);
				// if (!safeEpollCtl(EMPTY, requestNode, DEL, -1))
				// 	Logger::log("check-cgi: failed to delete fd from epoll", ERROR, false);
				close(requestNode->pipe_fds[READ_END]);
				return (1);
			}
		}
	}
	else
		return (0);
	requestNode->CGIReady = true;
	// Read the output from the child process
	requestNode->CGIBody.clear();
	while ((bytesRead = read(requestNode->pipe_fds[READ_END], buffer, sizeof(buffer) - 1)) > 0) {
		buffer[bytesRead] = '\0'; // Null-terminate the output
		// printf("CGI Output: %s", buffer); // Or store it in a variable if needed
		requestNode->CGIBody += buffer;
		if (requestNode->CGIBody.size() > MAX_BODY_SIZE)
		{		
			Logger::setErrorAndLog(&requestNode->CGIError, 413, "too large body from child");
			break ;
		}
	}
	// safeEpollCtl(EMPTY, requestNode, DEL, requestNode->pipe_fds[READ_END]);
	close(requestNode->pipe_fds[READ_END]);
	return (1);

}