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
# include <string>
# include <sys/socket.h>
# include <iostream>
# include <stdlib.h>
# include <unistd.h>
# include <netinet/in.h>
# include <sstream>
# include <arpa/inet.h>
# include <vector>
# include <sys/epoll.h>
# include <fcntl.h>
# include <sys/sendfile.h>
# include "../HttpParsing/HttpParser.hpp"
# include "../Config/ServerSettings.hpp"
#include <errno.h>
#include <string.h>
#include "../Response/Response.hpp"
#include <memory>
#include "../Response/ServerHandler.hpp"
#include <csignal>

# define MAX_EVENTS 20 //Can define this in config file or create a funct based on cpu load or leave it
# define TIME_OUT_PERIOD 10

class HttpServer
{
private:
	static HttpServer *_instance;
	std::string 	_ipAddress;
	int				_port;
	int				_serverFd;
	int 			_clientSocket;
	sockaddr_in 	_socketInfo; //reusable
	int				epollFd;
	epoll_event		_events;
	epoll_event		_eventsArr[MAX_EVENTS];
	int				numEvents;
	std::unordered_map<int, time_t> _fd_activity_map;
	
public:
	std::shared_ptr<ServerSettings> settings;
	//constructors & destructors
	HttpServer(std::shared_ptr<ServerSettings> _settings);
	~HttpServer();
	//operator overloads
	
	//methods
	static void signalHandler(int signal);
	void fdActivityLoop(const time_t);
	void startServer();
	void closeServer();
	void startListening();
};
