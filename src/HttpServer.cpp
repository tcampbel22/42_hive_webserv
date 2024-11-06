/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: clundber < clundber@student.hive.fi>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/04 10:18:33 by clundber          #+#    #+#             */
/*   Updated: 2024/11/06 14:54:09 by clundber         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/HttpServer.hpp"

void HttpServer::startServer()
{
	//make socket
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket == -1)
		return ; //change later
	std::cout << _socket << std::endl;
	
	//store socket addr info
	_socketInfo.sin_family = AF_INET; //macro for IPV4
	_socketInfo.sin_port = htons(_port); //converts port number to network byte order
	_socketInfo.sin_addr.s_addr = inet_addr(_ipAddress.c_str()); //converts ip address from string to uint

	//bind socket to port
	if (bind(_socket, (sockaddr *)&_socketInfo, sizeof(_socketInfo)) < 0)
		return ; //change later and remember to close all fds
	
	epollFd = epoll_create1(0);
	_events.events = EPOLLIN;
	_events.data.fd = _socket;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _socket, &_events) < 0)
		return ; //change later
}

void HttpServer::startListening()
{
	if (listen(_socket, 5) < 0)
	{
		return ; //change later, also close all fds
	}
	std::cout << "Server listening on port " << _port << std::endl;
	
	std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: 137\n\n<!DOCTYPE html>\n<html>\n<head>\n    <title>Simple C++ Web Server</title>\n</head>\n<body>\n    <h1>Hello from a C++ web server!</h1>\n</body>\n</html>\n";

	std::string response2 = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=UTF-8\nContent-Length: 137\n\n<!DOCTYPE html>\n<html>\n<head>\n    <title>Simple C++ Web Server</title>\n</head>\n<body>\n    <h1>Hello from a shitty C++ web server!</h1>\n</body>\n</html>\n";

	while (true)
	{
		numEvents = epoll_wait(epollFd, _eventsArr, MAX_EVENTS, 0);
		if (numEvents < 0)
		{
			//print error
			break ;
		}
		// sleep(1);
		//  std::cout << "numEvents = " << numEvents << std::endl;
		for (int i = 0; i < numEvents; i++)
		{
			if (_eventsArr[i].data.fd == _socket)
			{
				socklen_t _sockLen = sizeof(_socketInfo);
				_clientSocket.push_back(accept(_socket, (sockaddr *)&_socketInfo, &_sockLen));
				if (_clientSocket.back() < 0) 
				{
					//print error
					continue;
				}
				std::cout << "New client connected: " << _clientSocket[i] << std::endl;
				int	flag = fcntl(_clientSocket[i], F_GETFL, 0); //retrieves flags/settings from socket
				fcntl(_clientSocket[i], F_SETFL, flag | O_NONBLOCK); //Sets socket to be nonblocking
				
				epoll_event _clientEvents;
				_clientEvents.events = EPOLLIN | EPOLLOUT | EPOLLET;
				_clientEvents.data.fd = _clientSocket[i];
				
				if (epoll_ctl(epollFd, EPOLL_CTL_ADD, _clientSocket[i], &_clientEvents) < 0)
				{
					close (_clientSocket[i]);
					std::cout << "epoll_ctl error" << std::endl;
					continue;
				}
			}
			else
			{
				if (_clientSocket[i] % 2 == 0)
					send(_clientSocket[i], response.c_str(), response.size(), 0);
				else
					send(_clientSocket[i], response2.c_str(), response2.size(), 0);
			}
			
			//std::cout << "New client connected: " << _clientSocket[i] << std::endl;
		}
		
	}
	
	//close (_clientSocket);
	close (epollFd);
	

}

void HttpServer::closeServer()
{
	close(_socket);
}

HttpServer::~HttpServer()
{
	closeServer();
};
HttpServer::HttpServer(const std::string _ip, uint _newPort)
{
	_port = _newPort;
	_ipAddress = _ip;
	startServer();
	
};
