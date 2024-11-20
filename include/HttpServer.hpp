/************************************************/
/** __          __  _                          **/
/** \ \        / / | |                         **/
/**  \ \  /\  / /__| |__  ___  ___ _ ____   __ **/
/**   \ \/  \/ / _ \ '_ \/ __|/ _ \ '__\ \ / / **/
/**    \  /\  /  __/ |_) \__ \  __/ |   \ V /  **/
/**     \/  \/ \___|_.__/|___/\___|_|    \_/   **/
/**                                            **/
/**                                            **/
/**             W E B S E R V                  **/
/************************************************/

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
# include "../include/HttpParser.hpp"
#include <errno.h>
#include <string.h>
#include "../src/Response.hpp"
#include "../src/ServerHandler.hpp"
#include <csignal>

# define MAX_EVENTS 20 //Can define this in config file or create a funct based on cpu load or leave it

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
	
	
public:
	//constructors & destructors
	HttpServer(const std::string _ip, uint _port);
	~HttpServer();
	//operator overloads
	
	//methods
	static void signalHandler(int signal);
	void startServer();
	void closeServer();
	void startListening();
};
