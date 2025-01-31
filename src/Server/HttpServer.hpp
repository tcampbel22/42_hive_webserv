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

#pragma once
#include <string>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sstream>
#include <arpa/inet.h>
#include <vector>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include "../HttpParsing/HttpParser.hpp"
#include "../Config/ServerSettings.hpp"
#include <string.h>
#include "../Response/Response.hpp"
#include <memory>
#include "../Response/ServerHandler.hpp"
#include <csignal>
#include "../../include/webserv.hpp"
#include <sys/wait.h>

# define MAX_EVENTS 200 //Can define this in config file or create a funct based on cpu load or leave it
# define TIME_OUT_PERIOD 10
# define MAX_CONNECTIONS 500
# define TIME_OUT_MOD 0.011
#define READ_END 0
#define WRITE_END 1

enum e_poll
{
	E_IN,
	E_OUT,
	EMPTY,
	E_IO,
};

enum e_ctl
{
	ADD,
	MOD,
	DEL,
};

struct fdNode
{
	int				fd = -1;
	ServerSettings *serverPtr = nullptr;
	std::vector<char> _clientDataBuffer;
	bool			_connection = true;
	bool			_readyToSend = false;
	int				_error;
	bool			headerCorrect = false;
	//CGI stuff
	int				pipe_fds[2];
	bool			cgiStarted = false;
	pid_t 			pid = 0;
	std::string		childBody;
	std::string 	CGIBody;
	bool 			CGIReady = false;
	int				CGIError = 0;
	int				method = -1;
	std::string		path;
};

class HttpServer
{
private:
	float		_timeoutScale = 1.0;
	int			_connections = 3;
	static HttpServer	*_instance;
	std::vector<std::pair<std::string, int>> _ip_port_list;
	std::vector<int> _server_fds;
	std::vector<std::shared_ptr<fdNode>> server_nodes;
	std::map<int, std::shared_ptr<fdNode>> client_nodes;
	int 			_clientSocket; //Reused
	sockaddr_in 	_socketInfo; //reusable
	int				epollFd; //Only one instance
	epoll_event		_eventsArr[MAX_EVENTS];
	int				numEvents;
	std::unordered_map<int, time_t> _fd_activity_map;
	bool			_clientClosedConn = false;
	bool			requestComplete = false;
public:
	std::vector<ServerSettings> settings_vec;
	std::vector<std::pair<int, int>> pipe_vec;
	//constructors & destructors
	HttpServer(std::vector<ServerSettings> vec);
	~HttpServer();
	
	//methods
	static void signalHandler(int signal);
	void	fdActivityLoop(const time_t);
	bool	isRequestComplete(const std::vector<char>& data, ssize_t bytesReceived, std::shared_ptr<fdNode> node);
	bool	isChunkedTransferEncoding(const std::string& requestStr);
	bool	isRequestWithBody(std::string requestStr);
	bool	isMultiPart(std::string requestStr);
	int		getContentLength(const std::string& requestStr);
	void	startServer();
	void	startListening();
	void	acceptNewClient(ServerSettings* settingsPtr, int eventFd, time_t current_time);
	void	addServerToEpoll();
	void	fillHostPortPairs();
	void	readRequest(std::shared_ptr<fdNode> nodePtr);
	void	setNonBlocking(int socket);
	bool	isNonBlockingSocket(int fd);
	void	killNode(std::shared_ptr<fdNode> nodePtr);
	void	createClientNode(ServerSettings* settingsPtr);
	bool	checkSystemMemory(std::shared_ptr<fdNode> node);
	int		checkCGI(std::shared_ptr<fdNode>requestNode);
	bool	resetCGI(std::shared_ptr<fdNode> nodePtr);
	void	resetNode(std::shared_ptr<fdNode> nodePtr);
	bool	handle_read(std::shared_ptr<fdNode> nodePtr);
	bool	handle_write(std::shared_ptr<fdNode> nodePtr);
	void	validateHeaders(const std::vector<char>& data, int *errorFlag);
	void	cleanUpChild(std::shared_ptr<fdNode> nodePtr);
	bool	safeEpollCtl(e_poll event_type, std::shared_ptr<fdNode> node, e_ctl ctl, int fd);
	bool	validateContentLength(std::shared_ptr<fdNode> node, int length);
};
