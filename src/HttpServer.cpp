/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:18:33 by clundber          #+#    #+#             */
/*   Updated: 2024/11/07 10:58:19 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpServer.hpp"

void HttpServer::startServer()
{
	//make socket
	_serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverFd == -1)
		return ; //change later
	//std::cout << _serverFd << std::endl;
	
	//store socket addr info
	_socketInfo.sin_family = AF_INET; //macro for IPV4
	_socketInfo.sin_port = htons(_port); //converts port number to network byte order
	_socketInfo.sin_addr.s_addr = inet_addr(_ipAddress.c_str()); //converts ip address from string to uint

	//bind socket to port
	if (bind(_serverFd, (sockaddr *)&_socketInfo, sizeof(_socketInfo)) < 0)
		return ; //change later and remember to close all fds
	if (listen(_serverFd, 5) < 0)
	{
		return ; //change later, also close all fds
	}
	
	// epollFd = epoll_create1(0);
	// _events.events = EPOLLIN;
	// _events.data.fd = _serverFd;
	// if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _socket, &_events) < 0)
	// 	return ; //change later
}

void HttpServer::startListening()
{
	std::cout << "Server listening on port " << _port << std::endl;
	
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: 137\n\n<!DOCTYPE html>\n<html>\n<head>\n    <title>Simple C++ Web Server</title>\n</head>\n<body>\n    <h1>Hello from a C++ web server!</h1>\n</body>\n</html>\n";

	std::string response2 = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: 137\n\n<!DOCTYPE html>\n<html>\n<head>\n    <title>Simple C++ Web Server</title>\n</head>\n<body>\n    <h1>Hello from a shitty C++ web server!</h1>\n</body>\n</html>\n";

	
	 epollFd = epoll_create1(0);
	// int	flag = fcntl(_serverFd, F_GETFL, 0); //retrieves flags/settings from socket
	// fcntl(_serverFd, F_SETFL, flag | O_NONBLOCK); //Sets socket to be nonblocking
	_events.data.fd = _serverFd;
	 _events.events = EPOLLIN;
	epoll_ctl(epollFd, EPOLL_CTL_ADD, _serverFd, &_events); //maybe protect

	while (true)
	{
		numEvents = epoll_wait(epollFd, _eventsArr, MAX_EVENTS, 0);
		if (numEvents < 0){
			//print error
			break ;}
			
			// std::cout << numEvents << "\n";
		for (int i = 0; i < numEvents; i++)
		{
			std::cout << "got here" << std::endl;
			if (_eventsArr[i].data.fd == _serverFd)
			{
				socklen_t _sockLen = sizeof(_socketInfo);
				_clientSocket = accept(_serverFd, (sockaddr *)&_socketInfo, &_sockLen);
				if (_clientSocket < 0) 
				{
					
					//print error
					continue;
				}
				std::cout << "New client connected: " << _clientSocket << std::endl;

				int	flag = fcntl(_clientSocket, F_GETFL, 0); //retrieves flags/settings from socket
				fcntl(_clientSocket, F_SETFL, flag | O_NONBLOCK); //Sets socket to be nonblocking
                _events.events = EPOLLIN | EPOLLOUT | EPOLLET;
                _events.data.fd = _clientSocket;
				
				epoll_ctl(epollFd, EPOLL_CTL_ADD, _clientSocket, &_events); //guard later
			}
			else	
			{	
				int _fd_out = _eventsArr[i].data.fd;
				std::cout << "out fd =" << _fd_out << std::endl;
				if (_fd_out % 3 == 0)
					send(_fd_out, response.c_str(), response.size(), 0);
				else
					send(_fd_out, response2.c_str(), response2.size(), 0);
			}
			
		}
		
	}
	std::cout << "outofloop" << std::endl;
	//close (_clientSocket);
	close (epollFd);
	

}

void HttpServer::closeServer()
{
	close(_serverFd);
}

HttpServer::~HttpServer()
{
	closeServer();
};
HttpServer::HttpServer(const std::string _ip, uint _newPort)
{
	_port = _newPort;
	_ipAddress = _ip;
	_clientSocket = -1;
	startServer();
	
};
