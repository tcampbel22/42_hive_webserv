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
#include "../Config/LocationSettings.hpp"

CGIparsing::CGIparsing(std::string cgiPath) : _pathInfo(cgiPath) {
	_execInfo = "./root";
	_execInfo.append(cgiPath);
}

void CGIparsing::setCGIenvironment(HttpRequest& request, const std::string& queryStr) {
	(void)queryStr;
	setenv("REQUEST_METHOD", getMethod(request.method).c_str(), 1);
	//setenv("QUERY_STRING", queryStr.c_str(), 1); this can be commented out for now????
	setenv("CONTENT_TYPE", "text.html", 1); //default text, needs parsing for images etc.
	//setenv("CONTENT_LENGTH", request.headers.at("Content-Length").c_str(), 1);
	//setenv("SERVER_NAME", "localhost", 1); not doing names;
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

void CGIparsing::execute(HttpRequest& request, std::shared_ptr<LocationSettings>& cgiblock, int epollFd, epoll_event& _events) {
   int pipe_fds[2]; // File descriptors for the pipe
    pid_t pid;
    char buffer[1024]; // Buffer to hold the output from the CGI script
    ssize_t bytesRead;
	(void)cgiblock;
    // Create a pipe
    if (pipe(pipe_fds) == -1) {
		Logger::log("pipe error", ERROR, false);
        exit(1);
    }
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, pipe_fds[WRITE_END], &_events) == -1)
	{
		Logger::log("failed to add fd to epoll", ERROR, false);
		close(pipe_fds[READ_END]);
		close(pipe_fds[WRITE_END]);
		// delete client_node;
	}
	setToNonBlocking(pipe_fds[WRITE_END]);
    // Fork the child process
    pid = fork();
    if (pid == -1) 
	{
		Logger::log("fork error", ERROR, false);
        exit(1);
    }
	
    if (pid == 0) {
        // Child process

        // Close the write end of the pipe since the child will only write to stdout
        close(pipe_fds[READ_END]);

        // Redirect stdout to the write end of the pipe
        if (dup2(pipe_fds[WRITE_END], STDOUT_FILENO) == -1) {
            Logger::log("dup2 error", ERROR, false);
            exit(1);
        }

        // Close the write end of the pipe now that it's duplicated
        close(pipe_fds[WRITE_END]);
		const char *const argv[] = {_pathInfo.c_str(), nullptr};
        if (execve(_execInfo.c_str(), (char *const *)argv, environ) == -1) {
            Logger::log("execve error", ERROR, false);
            exit(1);
        }

    } else {
        // Parent process

        // Close the write end of the pipe since the parent will only read from the pipe
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, pipe_fds[WRITE_END], &_events) == -1)
		{
			Logger::log("failed to add fd to epoll", ERROR, false);
			close(pipe_fds[READ_END]);
			close(pipe_fds[WRITE_END]);
			// delete client_node;
		}
        close(pipe_fds[WRITE_END]);
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, pipe_fds[READ_END], &_events) == -1)
		{
			Logger::log("failed to add fd to epoll", ERROR, false);
			close(pipe_fds[READ_END]);
			close(pipe_fds[WRITE_END]);
			// delete client_node;
		}
        // Wait for the child process to finish
        waitpid(pid, NULL, 0);
        // Read the output from the child process
        while ((bytesRead = read(pipe_fds[READ_END], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0'; // Null-terminate the output
            //printf("CGI Output: %s", buffer); // Or store it in a variable if needed
			request.body += buffer;
        }
        // Close the read end of the pipe after reading
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, pipe_fds[READ_END], &_events) == -1)
		{
			Logger::log("failed to add fd to epoll", ERROR, false);
			close(pipe_fds[READ_END]);
			close(pipe_fds[WRITE_END]);
			// delete client_node;
		}
        close(pipe_fds[READ_END]);
    }
}

std::string CGIparsing::getPath() {
	return _pathInfo;
}

CGIparsing::~CGIparsing() {}