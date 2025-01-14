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
void	CGITimeout(pid_t &pid, int& errorCode)
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
			kill(pid, SIGKILL);
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		elapsed += interval;
	}
}

void CGIparsing::execute(HttpRequest& request, std::shared_ptr<LocationSettings>& cgiblock, int epollFd, epoll_event& _events) {
   int pipe_fds[2]; // File descriptors for the pipe
    pid_t pid;
    char buffer[1024]; // Buffer to hold the output from the CGI script
    ssize_t bytesRead;
	(void)cgiblock;
    // Create a pipe
    if (pipe(pipe_fds) == -1) {
        Logger::log("pipe: "  + (std::string)strerror(errno), ERROR, false);
        exit(1);
    }
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, pipe_fds[WRITE_END], &_events) == -1)
	{
		Logger::log("epoll_ctl: failed to add fd to epoll", ERROR, false);
		close(pipe_fds[READ_END]);
		close(pipe_fds[WRITE_END]);
		// delete client_node;
	}
	setToNonBlocking(pipe_fds[WRITE_END]);
    // Fork the child process
    pid = fork();
    if (pid == -1) {
        Logger::log("fork: " + (std::string)strerror(errno), ERROR, false);
        exit(1);
    }
	
    if (pid == 0) {
        // Child process

        // Close the write end of the pipe since the child will only write to stdout

        // Redirect stdout to the write end of the pipe
        if (dup2(pipe_fds[WRITE_END], STDOUT_FILENO) == -1) {
            Logger::log("dup2: " + (std::string)strerror(errno), ERROR, false);
            exit(1);
        }
		if (dup2(pipe_fds[READ_END], STDIN_FILENO) == -1) {
            Logger::log("dup2: "  + (std::string)strerror(errno), ERROR, false);
            exit(1);
        }

        close(pipe_fds[READ_END]);

        // Close the write end of the pipe now that it's duplicated
        close(pipe_fds[WRITE_END]);
		std::cerr << "exec info: " <<_execInfo << '\n';
		const char *const argv[] = {_scriptName.c_str(), nullptr};
        if (execve(_execInfo.c_str(), (char *const *)argv, environ) == -1) {
            Logger::log("execve: " + (std::string)strerror(errno), ERROR, false);
            exit(1);
        }

    } else {
		std::cout << _scriptName << std::endl;
        // Parent process
		if (request.method == 2) {  // POST method
			std::string body = request.body;
			request.body.clear();
            write(pipe_fds[WRITE_END], body.c_str(), body.size());
        }
        // Close the write end of the pipe since the parent will only read from the pipe
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, pipe_fds[WRITE_END], &_events) == -1)
		{
			Logger::log("epll_ctl: failed to delete fd", ERROR, false);
			close(pipe_fds[READ_END]);
			close(pipe_fds[WRITE_END]);
			// delete client_node;
		}
        close(pipe_fds[WRITE_END]);
		if (epoll_ctl(epollFd, EPOLL_CTL_ADD, pipe_fds[READ_END], &_events) == -1)
		{
			Logger::log("epoll_ctl: failed to add fd to epoll", ERROR, false);
			close(pipe_fds[READ_END]);
			close(pipe_fds[WRITE_END]);
			// delete client_node;
		}

		CGITimeout(pid, request.errorFlag); //Allow child process to finish or timeout

		waitpid(pid, NULL, 0); // Wait for the child process to finish
		
        // Read the output from the child process
		if (request.errorFlag == 0)
		{
			while ((bytesRead = read(pipe_fds[READ_END], buffer, sizeof(buffer) - 1)) > 0) {
				buffer[bytesRead] = '\0'; // Null-terminate the output
				//printf("CGI Output: %s", buffer); // Or store it in a variable if needed
				request.body += buffer;
			}
		}
		// Close the read end of the pipe after reading
		if (epoll_ctl(epollFd, EPOLL_CTL_DEL, pipe_fds[READ_END], &_events) == -1)
		{
			Logger::log("epoll_ctl: failed to add fd to epoll", ERROR, false);
			close(pipe_fds[READ_END]);
			close(pipe_fds[WRITE_END]);
			// delete client_node;
		}
        close(pipe_fds[READ_END]);
    }
}

std::string CGIparsing::getPath() {
	return _scriptName;
}

CGIparsing::~CGIparsing() {}