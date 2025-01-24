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

#include "CGIparsing.hpp"
#include "../HttpParsing/HttpParser.hpp"

CGIparsing::CGIparsing(std::string root, std::string script) {
	_scriptName = script.substr(script.find_last_of('/'));
	_execInfo = "." + root;
}

void CGIparsing::setCGIenvironment(HttpRequest& request, HttpParser& parser, LocationSettings& cgiBlock) {
	setenv("REQUEST_METHOD", getMethod(request.method).c_str(), 1);
	setenv("QUERY_STRING", parser.getQuery().c_str(), 1);
	if (request.headers.find("Content-Type") != request.headers.end())
		setenv("CONTENT_TYPE", request.headers.at("Content-Type").c_str(), 1); //default text, needs parsing for images etc.
	setenv("UPLOAD_DIR", cgiBlock.getCgiUploadPath().c_str(), 1);
	if (request.headers.find("Content-Length") != request.headers.end())
		setenv("CONTENT_LENGTH", request.headers.at("Content-Length").c_str(), 1);
	setenv("PATH_INFO", parser.getPathInfo().c_str(), 1);
	setenv("SERVER_NAME", request.headers.at("Host").c_str(), 1);
	setenv("SERVER_PORT", getPort(request.host).c_str(), 1);
	setenv("REMOTE_ADDR", getIp(request.host).c_str(), 1);
}

std::string CGIparsing::getMethod(int val) {
	switch (val)
	{
	case 1:
		return "GET";
	case 2:
		return "POST";
	default:
		break;
	}
	return (NULL);
}

std::string CGIparsing::getIp(std::string& host) {
	size_t colPos = host.find(':');
	if (colPos != std::string::npos)
		return host.substr(0, colPos);
	return nullptr;	
}

std::string CGIparsing::getPort(std::string& host) {
	size_t colPos = host.find(':');
	if (colPos != std::string::npos)
		return host.substr(colPos + 1);
	return nullptr;
}

void	setToNonBlocking(int socket)
{
	int	flag = fcntl(socket, F_GETFL, 0); //retrieves flags/settings from socket
	fcntl(socket, F_SETFL, flag | O_NONBLOCK); //Sets socket to be nonblocking
}

//checks if the child process (CGI) has finished each iteration, if it take stoo long, then it send a kill command to the child process
void	CGITimeout(pid_t &pid, int& errorCode, int* pipe_fds)
{
	int elapsed = 0;
	int	interval = 10; //milliseconds
	pid_t result;
    int status;

	while (1)
	{
		result = waitpid(pid, &status, WNOHANG);
		if (result == pid)
		{
			if (WIFEXITED(status))
			{
				if(WEXITSTATUS(status))
					Logger::setErrorAndLog(&errorCode, 502, "child process failed");
			}
			break;
		}
		if (elapsed > CGI_TIMEOUT)
		{
			Logger::setErrorAndLog(&errorCode, 504, "Child process teminated due timeout");
			close(pipe_fds[WRITE_END]);
			close(pipe_fds[READ_END]);
			kill(pid, SIGKILL);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		elapsed += interval;
	}
}

void CGIparsing::execute(HttpRequest& request, std::shared_ptr<LocationSettings>& cgiblock, int epollFd, epoll_event& _events, std::vector<std::pair<int, int>>& pipe_vec, fdNode *requestNode) {
   	// int pipe_fds[2]; // File descriptors for the pipe
    // pid_t pid;
	(void)cgiblock;
	(void)epollFd;
	(void)_events;
    // Create a pipe
	if (pipe(requestNode->pipe_fds) == -1) {
        Logger::log("pipe: failed to open", ERROR, false);
        // exit(1); needs to be something else
    }
	// if (epoll_ctl(epollFd, EPOLL_CTL_ADD, pipe_fds[WRITE_END], &_events) == -1)
	// {
	// 	Logger::log("epoll_ctl: failed to add fd to epoll", ERROR, false);
	// 	// delete client_node;
	// }
	setToNonBlocking(requestNode->pipe_fds[WRITE_END]);
	setToNonBlocking(requestNode->pipe_fds[READ_END]);
	pipe_vec.emplace_back(requestNode->pipe_fds[WRITE_END], requestNode->pipe_fds[READ_END]); //probably not needed
    // Fork the child process
    requestNode->pid = fork();
    if (requestNode->pid == -1) {
        Logger::log("fork: failed to fork", ERROR, false);
        exit(1);
    }
	
    if (requestNode->pid == 0) {
        // Child process

        // Close the write end of the pipe since the child will only write to stdout
        // Redirect stdout to the write end of the pipe
        if (dup2(requestNode->pipe_fds[WRITE_END], STDOUT_FILENO) == -1) {
            Logger::log("dup2: failed", ERROR, false);
            exit(1);
        }
		if (dup2(requestNode->pipe_fds[READ_END], STDIN_FILENO) == -1) {
            Logger::log("dup2: failed", ERROR, false);
            exit(1);
        }

        close(requestNode->pipe_fds[READ_END]);

        // Close the write end of the pipe now that it's duplicated
		if (request.method == 2) {  // POST method
			ssize_t bytesRecieved = write(requestNode->pipe_fds[WRITE_END], request.body.c_str(), request.body.size());
			close(requestNode->pipe_fds[WRITE_END]);
            if (bytesRecieved == -1 || bytesRecieved == 0) {
				exit(1);
			}
        }
		const char *const argv[] = {_scriptName.c_str(), nullptr};
        if (execve(_execInfo.c_str(), (char *const *)argv, environ) == -1) 
		{
            Logger::log("execve: failed to execute command", ERROR, false);
            close(requestNode->pipe_fds[WRITE_END]);
			cgiblock.reset();
			exit(1);
        }

    } else {
		requestNode->cgiStarted = true;
        // Parent process
        // Close the write end of the pipe since the parent will only read from the pipe
		// if (epoll_ctl(epollFd, EPOLL_CTL_DEL, pipe_fds[WRITE_END], &_events) == -1)
		// {
		// 	Logger::log("epll_ctl: failed to delete fd", ERROR, false);
		// 	close(pipe_fds[READ_END]);
		// 	close(pipe_fds[WRITE_END]);
		// 	// delete client_node;
		// }
        close(requestNode->pipe_fds[WRITE_END]);
		// if (epoll_ctl(epollFd, EPOLL_CTL_ADD, pipe_fds[READ_END], &_events) == -1)
		// {
		// 	Logger::log("epoll_ctl: failed to add fd to epoll", ERROR, false);
		// 	close(pipe_fds[READ_END]);
		// 	close(pipe_fds[WRITE_END]);
		// 	// delete client_node;
		// }

		// CGITimeout(requestNode->pid, request.errorFlag, requestNode->pipe_fds); //Allow child process to finish or timeout

		// // waitpid(pid, NULL, 0); // Wait for the child process to finish
		
        // // Read the output from the child process
		// request.body.clear();
		// if (request.errorFlag == 0)
		// {
		// 	while ((bytesRead = read(requestNode->pipe_fds[READ_END], buffer, sizeof(buffer) - 1)) > 0) {
		// 		buffer[bytesRead] = '\0'; // Null-terminate the output
		// 		//printf("CGI Output: %s", buffer); // Or store it in a variable if needed
		// 		request.body += buffer;
		// 	}
		// }
		// // Close the read end of the pipe after reading
		// // if (epoll_ctl(epollFd, EPOLL_CTL_DEL, pipe_fds[READ_END], &_events) == -1)
		// // {
		// // 	Logger::log("epoll_ctl: failed to add fd to epoll", ERROR, false);
		// // 	close(pipe_fds[READ_END]);
		// // 	close(pipe_fds[WRITE_END]);
		// // 	// delete client_node;
		// // }
        // close(requestNode->pipe_fds[READ_END]);
    }
}

// int CGIparsing::checkCGI(fdNode *requestNode)
// {
// 	char 	buffer[1024]; //CGI buffer
// 	ssize_t bytesRead = 0; //for CGI reading

//    		int status;
// 		pid_t result = waitpid(requestNode->pid, &status, WNOHANG);
// 		if (result == requestNode->pid)
// 		{
// 			if (WIFEXITED(status))
// 			{
// 				if(WEXITSTATUS(status))
// 				{
// 					Logger::setErrorAndLog(&requestNode->CGIError, 502, "child process failed");
// 					requestNode->CGIReady = true;
// 					close(requestNode->pipe_fds[READ_END]);
// 					return (1);
// 				}
// 			}
// 		}
// 		else
// 			return (0);
// 		requestNode->CGIReady = true;
//         // Read the output from the child process
// 		requestNode->CGIBody.clear();
// 		while ((bytesRead = read(requestNode->pipe_fds[READ_END], buffer, sizeof(buffer) - 1)) > 0) {
// 			buffer[bytesRead] = '\0'; // Null-terminate the output
// 			//printf("CGI Output: %s", buffer); // Or store it in a variable if needed
// 			requestNode->CGIBody += buffer;
// 			if (requestNode->CGIBody.size() > MAX_BODY_SIZE)
// 			{		
// 				Logger::setErrorAndLog(&requestNode->CGIError, 413, "too large body from child");
// 				break ;
// 			}
// 		}
//         close(requestNode->pipe_fds[READ_END]);
// 		return (1);

// }

std::string CGIparsing::getPath() {
	return _scriptName;
}